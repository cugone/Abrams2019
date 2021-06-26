#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Audio/Wav.hpp"
#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IFileLoggerService.hpp"

#include <algorithm>

AudioSystem::AudioSystem(std::size_t max_channels /*= 1024*/)
: EngineSubsystem()
, IAudioService()
, _max_channels(max_channels) {
    bool co_init_succeeded = SUCCEEDED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    GUARANTEE_OR_DIE(co_init_succeeded, "Failed to setup Audio System.");
    bool xaudio2_create_succeeded = SUCCEEDED(::XAudio2Create(&_xaudio2));
    GUARANTEE_OR_DIE(xaudio2_create_succeeded, "Failed to create Audio System.");
    ::X3DAudioInitialize(2, 343.0f, _x3daudio);
}

AudioSystem::~AudioSystem() noexcept {
    {
        std::scoped_lock<std::mutex> lock(_cs);
        for(auto& channel : _active_channels) {
            channel->Stop();
        }
    }
    {
        bool done_cleanup = false;
        do {
            std::this_thread::yield();
            std::scoped_lock<std::mutex> lock(_cs);
            done_cleanup = _active_channels.empty();
        } while(!done_cleanup);
    }

    _active_channels.clear();
    _active_channels.shrink_to_fit();

    _idle_channels.clear();
    _idle_channels.shrink_to_fit();

    _sounds.clear();
    _wave_files.clear();

    if(_master_voice) {
        _master_voice->DestroyVoice();
        _master_voice = nullptr;
    }

    if(_xaudio2) {
        _xaudio2->UnregisterForCallbacks(&_engine_callback);

        _xaudio2->Release();
        _xaudio2 = nullptr;
    }

    ::CoUninitialize();
}

void AudioSystem::Initialize() {
#ifdef AUDIO_DEBUG
    XAUDIO2_DEBUG_CONFIGURATION config{};
    config.LogFileline = true;
    config.LogFunctionName = true;
    config.LogThreadID = true;
    config.LogTiming = true;
    config.BreakMask = XAUDIO2_LOG_WARNINGS;
    config.TraceMask = XAUDIO2_LOG_DETAIL | XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_FUNC_CALLS;
    _xaudio2->SetDebugConfiguration(&config);
#endif
    _xaudio2->CreateMasteringVoice(&_master_voice);
    _idle_channels.reserve(_max_channels);
    _active_channels.reserve(_max_channels);

    FileUtils::Wav::WavFormatChunk fmt{};
    fmt.formatId = 1;
    fmt.channelCount = 1;
    fmt.samplesPerSecond = 44100;
    fmt.bytesPerSecond = 88200;
    fmt.dataBlockSize = 2;
    fmt.bitsPerSample = 16;
    SetFormat(fmt);

    SetEngineCallback(&_engine_callback);
}

AudioSystem::ChannelGroup* AudioSystem::GetChannelGroup(const std::string& name) const noexcept {
    if(const auto found = std::find_if(std::cbegin(_channel_groups), std::cend(_channel_groups), [&name](const auto& group) { return group.first.string() == name; }); found != std::cend(_channel_groups)) {
        return found->second.get();
    }
    return nullptr;
}

const std::atomic_uint32_t& AudioSystem::GetOperationSetId() const noexcept {
    return _operationID;
}
const std::atomic_uint32_t& AudioSystem::IncrementAndGetOperationSetId() noexcept {
    IncrementOperationSetId();
    return GetOperationSetId();
}

void AudioSystem::IncrementOperationSetId() noexcept {
    _operationID++;
}

void AudioSystem::SubmitDeferredOperation(uint32_t operationSetId) noexcept {
    _xaudio2->CommitChanges(operationSetId);
}

void AudioSystem::AddChannelGroup(const std::string& name) noexcept {
    if(const auto found = std::find_if(std::cbegin(_channel_groups), std::cend(_channel_groups), [&name](const auto& group) { return group.first.string() == name; }); found != std::cend(_channel_groups)) {
        return;
    }
    auto group = std::make_unique<ChannelGroup>(*this, name);
    auto channel = std::make_unique<Channel>(*this, AudioSystem::Channel::ChannelDesc{});
    auto* channel_ptr = channel.get();
    _idle_channels.push_back(std::move(channel));
    group->AddChannel(channel_ptr);
    _channel_groups.emplace_back(std::make_pair(name, std::move(group)));
}

void AudioSystem::RemoveChannelGroup(const std::string& name) noexcept {
    if(auto found = std::find_if(std::begin(_channel_groups), std::end(_channel_groups), [&name](const auto& group) { return group.first.string() == name; }); found != std::end(_channel_groups)) {
        for(auto* channel : found->second->channels) {
            DeactivateChannel(*channel);
        }
        found->second.reset(nullptr);
        _channel_groups.erase(found);
    }
}

void AudioSystem::AddSoundToChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept {
    if(!snd) {
        return;
    }
    if(auto* group = GetChannelGroup(channelGroupName); group != nullptr) {
        for(auto* channel : group->channels) {
            snd->AddChannel(channel);
        }
        for(auto* channel : snd->GetChannels()) {
            group->AddChannel(channel);
        }
    }
}

void AudioSystem::AddSoundToChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept {
    if(auto group = GetChannelGroup(channelGroupName); group != nullptr) {
        auto* snd = CreateSound(filepath);
        AddSoundToChannelGroup(channelGroupName, snd);
    }
}

void AudioSystem::RemoveSoundFromChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept {
    if(!snd) {
        return;
    }
    if(auto group = GetChannelGroup(channelGroupName); group != nullptr) {
        for(auto* channel : snd->GetChannels()) {
            group->RemoveChannel(channel);
        }
        for(auto* channel : group->channels) {
            snd->RemoveChannel(channel);
        }
    }
}

void AudioSystem::RemoveSoundFromChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept {
    if(const auto group = GetChannelGroup(channelGroupName); group != nullptr) {
        if(const auto found_iter = std::find_if(std::begin(_sounds), std::end(_sounds), [&filepath](const auto& a) { return a.first == filepath;  }); found_iter != std::end(_sounds)) {
            auto* snd = found_iter->second.get();
            RemoveSoundFromChannelGroup(channelGroupName, snd);
        }
    }
}

void AudioSystem::SetEngineCallback(EngineCallback* callback) noexcept {
    if(&_engine_callback == callback) {
        return;
    }
    _xaudio2->UnregisterForCallbacks(&_engine_callback);
    _engine_callback = *callback;
    _xaudio2->RegisterForCallbacks(&_engine_callback);
}

const WAVEFORMATEXTENSIBLE& AudioSystem::GetFormat() const noexcept {
    return _audio_format_ex;
}

FileUtils::Wav::WavFormatChunk AudioSystem::GetLoadedWavFileFormat() const noexcept {
    FileUtils::Wav::WavFormatChunk fmt{};
    if(_wave_files.empty()) {
        return fmt;
    }
    return _wave_files.begin()->second->GetFormatChunk();
}

void AudioSystem::BeginFrame() {
    /* DO NOTHING */
}

void AudioSystem::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) {
    /* DO NOTHING */
}

void AudioSystem::Render() const {
    /* DO NOTHING */
}

void AudioSystem::EndFrame() {
    std::scoped_lock<std::mutex> lock(_cs);
    _idle_channels.erase(std::remove_if(std::begin(_idle_channels), std::end(_idle_channels), [](const std::unique_ptr<Channel>& c) { return c == nullptr; }), std::end(_idle_channels));
    _active_channels.erase(std::remove_if(std::begin(_active_channels), std::end(_active_channels), [](const std::unique_ptr<Channel>& c) { return c == nullptr; }), std::end(_active_channels));
}

bool AudioSystem::ProcessSystemMessage(const EngineMessage& /*msg*/) noexcept {
    return false;
}

void AudioSystem::SuspendAudio() noexcept {
    if(_xaudio2) {
        _xaudio2->StopEngine();
    }
}

void AudioSystem::ResumeAudio() noexcept {
    if(_xaudio2) {
        _xaudio2->StartEngine();
    }
}

void AudioSystem::SetFormat(const WAVEFORMATEXTENSIBLE& format) noexcept {
    _audio_format_ex = format;
}

void AudioSystem::SetFormat(const FileUtils::Wav::WavFormatChunk& format) noexcept {
    auto fmt_buffer = reinterpret_cast<const unsigned char*>(&format);
    std::memcpy(&_audio_format_ex, fmt_buffer, sizeof(_audio_format_ex));
}

void AudioSystem::RegisterWavFilesFromFolder(std::filesystem::path folderpath, bool recursive /*= false*/) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(folderpath)) {
        DebuggerPrintf("Attempting to Register Wav Files from unknown path: %s\n", FS::absolute(folderpath).string().c_str());
        return;
    }
    folderpath = FS::canonical(folderpath);
    folderpath.make_preferred();
    if(!FS::is_directory(folderpath)) {
        return;
    }
    const auto cb =
    [this](const std::filesystem::path& p) {
        RegisterWavFile(p);
    };
    FileUtils::ForEachFileInFolder(folderpath, ".wav", cb, recursive);
}

void AudioSystem::DeactivateChannel(Channel& channel) noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    const auto found_iter = std::find_if(std::begin(_active_channels), std::end(_active_channels),
                                         [&channel](const std::unique_ptr<Channel>& c) { return c.get() == &channel; });
    _active_channels.erase(found_iter);
}

void AudioSystem::Play(Sound& snd, SoundDesc desc /* = SoundDesc{}*/) noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    if(_max_channels <= _idle_channels.size()) {
        return;
    }
    if(_max_channels <= _active_channels.size()) {
        return;
    }
    if(auto* group = GetChannelGroup(desc.groupName)) {
        const auto groupvolume = group->GetVolume();
        if(desc.volume == 1.0f) {
            desc.volume = groupvolume;
        }
    }
    if(MathUtils::IsEquivalentToZero(desc.volume)) {
        return;
    }
    auto channelDesc = AudioSystem::Channel::ChannelDesc{this};
    channelDesc = desc;
    _idle_channels.push_back(std::make_unique<Channel>(*this, channelDesc));
    _active_channels.push_back(std::move(_idle_channels.back()));
    auto& inserted_channel = _active_channels.back();
    inserted_channel->Play(snd);
}

void AudioSystem::Play(std::filesystem::path filepath, SoundDesc desc /*= SoundDesc{}*/) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        return;
    }
    Sound* snd = CreateSound(filepath);
    Play(*snd, desc);
}

void AudioSystem::Play(const std::filesystem::path& filepath) noexcept {
    Play(filepath, SoundDesc{});
}

void AudioSystem::Play(const std::size_t id) noexcept {
    Play(_sounds[id].first, SoundDesc{});
}

void AudioSystem::Play(const std::filesystem::path& filepath, const bool looping) noexcept {
    SoundDesc desc{};
    desc.loopCount = looping ? -1 : 0;
    Play(filepath, desc);
}

void AudioSystem::Play(const std::size_t id, const bool looping) noexcept {
    SoundDesc desc{};
    desc.loopCount = looping ? -1 : 0;
    Play(_sounds[id].first, desc);
}

void AudioSystem::Stop(const std::filesystem::path& filepath) noexcept {
    const auto& found = std::find_if(std::cbegin(_sounds), std::cend(_sounds), [&filepath](const auto& snd) { return snd.first == filepath; });
    if(found != std::cend(_sounds)) {
        for(auto& channel : found->second->GetChannels()) {
            channel->Stop();
            DeactivateChannel(*channel);
        }
    }
}

void AudioSystem::Stop(const std::size_t id) noexcept {
    auto& channel = _active_channels[id];
    channel->Stop();
    DeactivateChannel(*channel);
}

void AudioSystem::StopAll() noexcept {
    const auto& op_id = IncrementAndGetOperationSetId();
    for(auto& active_sound : _active_channels) {
        active_sound->Stop(op_id);
        DeactivateChannel(*active_sound);
    }
    SubmitDeferredOperation(op_id);
}

AudioSystem::Sound* AudioSystem::CreateSound(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        DebuggerPrintf("Could not find file: %s\n", filepath.string().c_str());
        return nullptr;
    }

    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    const auto finder = [&filepath](const auto& a) { return a.first == filepath; };
    auto found_iter = std::find_if(std::begin(_sounds), std::end(_sounds), finder);
    if(found_iter == _sounds.end()) {
        _sounds.emplace_back(std::make_pair(filepath, std::move(std::make_unique<Sound>(*this, filepath))));
        found_iter = std::find_if(std::begin(_sounds), std::end(_sounds), finder);
    }
    return found_iter->second.get();
}

AudioSystem::Sound* AudioSystem::CreateSoundInstance(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        DebuggerPrintf("Could not find file: %s\n", filepath.string().c_str());
        return nullptr;
    }
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    _sounds.emplace_back(std::make_pair(filepath, std::move(std::make_unique<Sound>(*this, filepath))));
    return _sounds.back().second.get();
}

void AudioSystem::RegisterWavFile(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        DebuggerPrintf("Attempting to register wav file that does not exist: %s\n", filepath.string().c_str());
        return;
    }
    filepath = FS::canonical(filepath);
    if(const auto found = std::find_if(std::cbegin(_wave_files), std::cend(_wave_files), [&filepath](const auto& wav) { return wav.first == filepath; }); found != std::cend(_wave_files)) {
        return;
    }

    if(const auto wav_result = [&]() {
           auto&& wav = std::make_unique<FileUtils::Wav>();
           if(const auto result = wav->Load(filepath); result == FileUtils::Wav::WAV_SUCCESS) {
               _wave_files.emplace_back(std::make_pair(filepath, std::move(wav)));
               return result;
           } else {
               return result;
           }
       }(); //IIIL
       wav_result != FileUtils::Wav::WAV_SUCCESS) {
        switch(wav_result) {
        case FileUtils::Wav::WAV_ERROR_NOT_A_WAV: {
            DebuggerPrintf("%s is not a .wav file.\n", filepath.string().c_str());
            break;
        }
        case FileUtils::Wav::WAV_ERROR_BAD_FILE: {
            DebuggerPrintf("%s is improperly formatted.\n", filepath.string().c_str());
            break;
        }
        default: {
            DebuggerPrintf("Unknown error attempting to load %s\n", filepath.string().c_str());
            break;
        }
        }
    }
}

void STDMETHODCALLTYPE AudioSystem::Channel::VoiceCallback::OnBufferEnd(void* pBufferContext) {
    Channel& channel = *static_cast<Channel*>(pBufferContext);
    channel.Stop();
    channel._sound->RemoveChannel(&channel);
    channel._sound = nullptr;
    channel._audio_system->DeactivateChannel(channel);
}

void STDMETHODCALLTYPE AudioSystem::Channel::VoiceCallback::OnLoopEnd(void* pBufferContext) {
    Channel& channel = *static_cast<Channel*>(pBufferContext);
    if(channel._desc.stopWhenFinishedLooping && channel._desc.loop_count != XAUDIO2_LOOP_INFINITE) {
        if(++channel._desc.repeat_count >= channel._desc.loop_count) {
            channel.Stop();
        }
    }
}

AudioSystem::Channel::Channel(AudioSystem& audioSystem, const ChannelDesc& desc) noexcept
: _audio_system(&audioSystem)
, _desc{desc} {
    static VoiceCallback vcb;
    _buffer.pContext = this;
    auto fmt = reinterpret_cast<const WAVEFORMATEX*>(&(_audio_system->GetFormat()));
    _audio_system->_xaudio2->CreateSourceVoice(&_voice, fmt, 0, _desc.frequency_max, &vcb);
    if(auto* group = _audio_system->GetChannelGroup(desc.groupName); group != nullptr) {
        group->AddChannel(this);
    }
}

AudioSystem::Channel::~Channel() noexcept {
    if(_voice) {
        Stop();
        {
            std::scoped_lock<std::mutex> lock(_cs);
            _voice->DestroyVoice();
            _voice = nullptr;
        }
    }
}

void AudioSystem::Channel::Play(Sound& snd) noexcept {
    snd.AddChannel(this);
    _sound = &snd;
    if(const auto* wav = snd.GetWav()) {
        _buffer.pAudioData = wav->GetDataBuffer();
        _buffer.AudioBytes = wav->GetDataBufferSize();
        _buffer.LoopCount = _desc.loop_count;
        _buffer.LoopBegin = 0;
        _buffer.LoopLength = 0;
        if(_desc.loop_count) {
            _buffer.LoopBegin = _desc.loop_beginSamples;
            _buffer.LoopLength = _desc.loop_endSamples - _desc.loop_beginSamples;
        }
        {
            std::scoped_lock<std::mutex> lock(_cs);
            _voice->SubmitSourceBuffer(&_buffer, nullptr);
            _voice->SetVolume(_desc.volume);
            _voice->SetFrequencyRatio(_desc.frequency);
            _voice->Start();
        }
    }
}

void AudioSystem::Channel::Play(Sound& snd, uint32_t operationSetId) noexcept {
    snd.AddChannel(this);
    _sound = &snd;
    if(const auto* wav = snd.GetWav()) {
        _buffer.pAudioData = wav->GetDataBuffer();
        _buffer.AudioBytes = wav->GetDataBufferSize();
        _buffer.LoopCount = _desc.loop_count;
        _buffer.LoopBegin = 0;
        _buffer.LoopLength = 0;
        if(_desc.loop_count) {
            _buffer.LoopBegin = _desc.loop_beginSamples;
            _buffer.LoopLength = _desc.loop_endSamples - _desc.loop_beginSamples;
        }
        {
            std::scoped_lock<std::mutex> lock(_cs);
            _voice->SubmitSourceBuffer(&_buffer, nullptr);
            _voice->SetVolume(_desc.volume, operationSetId);
            _voice->SetFrequencyRatio(_desc.frequency, operationSetId);
            _voice->Start(0, operationSetId);
        }
    }
}

void AudioSystem::Channel::Stop() noexcept {
    if(_voice) {
        std::scoped_lock<std::mutex> lock(_cs);
        _voice->Stop();
        _voice->FlushSourceBuffers();
    }
}

void AudioSystem::Channel::Stop(uint32_t operationSetId) noexcept {
    if(_voice) {
        std::scoped_lock<std::mutex> lock(_cs);
        _voice->Stop(XAUDIO2_PLAY_TAILS, operationSetId);
        _voice->FlushSourceBuffers();
    }
}

void AudioSystem::Channel::Pause() noexcept {
    if(_voice) {
        std::scoped_lock<std::mutex> lock(_cs);
        _voice->Stop();
    }
}

void AudioSystem::Channel::Pause(uint32_t operationSetId) noexcept {
    if(_voice) {
        std::scoped_lock<std::mutex> lock(_cs);
        _voice->Stop(0, operationSetId);
    }
}

AudioSystem::Channel::ChannelDesc::ChannelDesc(AudioSystem* audioSystem)
: audio_system{audioSystem} {
    /* DO NOTHING */
}

AudioSystem::Channel::ChannelDesc& AudioSystem::Channel::ChannelDesc::operator=(const SoundDesc& sndDesc) {
    volume = sndDesc.volume;
    frequency = sndDesc.frequency;
    loop_count = sndDesc.loopCount <= -1 ? XAUDIO2_LOOP_INFINITE : (std::clamp(sndDesc.loopCount, 0, XAUDIO2_MAX_LOOP_COUNT));
    stopWhenFinishedLooping = sndDesc.stopWhenFinishedLooping;
    const auto& fmt = audio_system->GetLoadedWavFileFormat();
    loop_beginSamples = static_cast<uint32_t>(fmt.samplesPerSecond * sndDesc.loopBegin.count());
    loop_endSamples = static_cast<uint32_t>(fmt.samplesPerSecond * sndDesc.loopEnd.count());
    groupName = sndDesc.groupName;
    return *this;
}

void AudioSystem::Channel::SetStopWhenFinishedLooping(bool value) {
    _desc.stopWhenFinishedLooping = value;
}

void AudioSystem::Channel::SetLoopCount(int count) noexcept {
    if(count <= -1) {
        _desc.loop_count = XAUDIO2_LOOP_INFINITE;
    } else {
        count = std::clamp(count, 0, XAUDIO2_MAX_LOOP_COUNT);
        _desc.loop_count = count;
    }
}

uint32_t AudioSystem::Channel::GetLoopCount() const noexcept {
    return _desc.loop_count;
}

void AudioSystem::Channel::SetLoopRange(TimeUtils::FPSeconds start, TimeUtils::FPSeconds end) {
    SetLoopBegin(start);
    SetLoopEnd(end);
}

void AudioSystem::Channel::SetLoopBegin(TimeUtils::FPSeconds start) {
    const auto& fmt = _audio_system->GetLoadedWavFileFormat();
    _desc.loop_beginSamples = static_cast<uint32_t>(fmt.samplesPerSecond * start.count());
}

void AudioSystem::Channel::SetLoopEnd(TimeUtils::FPSeconds end) {
    const auto& fmt = _audio_system->GetLoadedWavFileFormat();
    _desc.loop_endSamples = static_cast<uint32_t>(fmt.samplesPerSecond * end.count());
}

void AudioSystem::Channel::SetVolume(float newVolume) noexcept {
    _desc.volume = newVolume;
}

void AudioSystem::Channel::SetFrequency(float newFrequency) noexcept {
    newFrequency = std::clamp(newFrequency, XAUDIO2_MIN_FREQ_RATIO, _desc.frequency_max);
    _desc.frequency = newFrequency;
}

float AudioSystem::Channel::GetVolume() const noexcept {
    return _desc.volume;
}

float AudioSystem::Channel::GetFrequency() const noexcept {
    return _desc.frequency;
}

AudioSystem::Sound::Sound(AudioSystem& audiosystem, std::filesystem::path filepath)
: _audio_system(&audiosystem) {
    namespace FS = std::filesystem;
    GUARANTEE_OR_DIE(FS::exists(filepath), "Attempting to create sound that does not exist.\n");
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    const auto pred = [&filepath](const auto& wav) { return wav.first == filepath; };
    auto found = std::find_if(std::begin(_audio_system->_wave_files), std::end(_audio_system->_wave_files), pred);
    if(found == _audio_system->_wave_files.end()) {
        _audio_system->RegisterWavFile(filepath);
        found = std::find_if(std::begin(_audio_system->_wave_files), std::end(_audio_system->_wave_files), pred);
    }
    if(found != std::end(_audio_system->_wave_files)) {
        _my_id = _id++;
        _wave_file = found->second.get();
    }
}

void AudioSystem::Sound::AddChannel(Channel* channel) noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    _channels.push_back(channel);
}

void AudioSystem::Sound::RemoveChannel(Channel* channel) noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    _channels.erase(std::remove_if(std::begin(_channels), std::end(_channels),
                                   [channel](Channel* c) -> bool { return c == channel; }),
                    std::end(_channels));
}

const std::size_t AudioSystem::Sound::GetId() const noexcept {
    return _my_id;
}

const std::size_t AudioSystem::Sound::GetCount() noexcept {
    return _id;
}

const FileUtils::Wav* const AudioSystem::Sound::GetWav() const noexcept {
    return _wave_file;
}

const std::vector<AudioSystem::Channel*>& AudioSystem::Sound::GetChannels() const noexcept {
    return _channels;
}

void STDMETHODCALLTYPE AudioSystem::EngineCallback::OnCriticalError(HRESULT error) {
    std::ostringstream ss;
    ss << "The Audio System encountered a fatal error: ";
    ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << error;
    ERROR_AND_DIE(ss.str().c_str());
}

AudioSystem::ChannelGroup::ChannelGroup(AudioSystem& parentAudioSystem) noexcept
: AudioSystem::ChannelGroup::ChannelGroup(parentAudioSystem, _name) {
}

AudioSystem::ChannelGroup::ChannelGroup(AudioSystem& parentAudioSystem, std::string name) noexcept
: _audio_system{parentAudioSystem}
, _name{name} {
    const auto sampleRate = _audio_system.GetLoadedWavFileFormat().samplesPerSecond;
    const auto channelCount = _audio_system.GetLoadedWavFileFormat().channelCount;
    auto hr = _audio_system._xaudio2->CreateSubmixVoice(&_groupVoice, channelCount, sampleRate, 0, 0, nullptr, nullptr);
    const auto error_msg = [&]() {
        std::string result{"AudioSystem failed to create channel group " + name + "\nError:\n"};
        switch(hr) {
        case XAUDIO2_E_INVALID_CALL:
            result += "Invalid Call. Check run-time parameters.";
            break;
        case XAUDIO2_E_XMA_DECODER_ERROR:
            result += "The Xbox 360 XMA hardware suffered an unrecoverable error.";
            break;
        case XAUDIO2_E_XAPO_CREATION_FAILED:
            result += "An effect failed to instantiate.";
            break;
        case XAUDIO2_E_DEVICE_INVALIDATED:
            result += "An audio device became unusable through being unplugged or some other event.";
            break;
        }
        return result;
    }(); //IIIL
    GUARANTEE_OR_DIE(hr == S_OK, error_msg.c_str());
}

void AudioSystem::ChannelGroup::AddChannel(Channel* channel) noexcept {
    if(channel == nullptr || channel->_voice == nullptr) {
        return;
    }
    {
        std::scoped_lock lock(_cs);
        auto SFXSend = XAUDIO2_SEND_DESCRIPTOR{0, _groupVoice};
        auto SFXSendList = XAUDIO2_VOICE_SENDS{1, &SFXSend};
        channel->_voice->SetOutputVoices(&SFXSendList);
    }
    channels.push_back(channel);
}

void AudioSystem::ChannelGroup::RemoveChannel(Channel* channel) noexcept {
    if(channel == nullptr || channel->_voice == nullptr) {
        return;
    }
    {
        std::scoped_lock lock(_cs);
        channel->_voice->SetOutputVoices(nullptr); //Removes from group, still sends to master
    }
    if(auto found = std::find(std::begin(channels), std::end(channels), channel); found != std::end(channels)) {
        channels.push_back(channel);
    }
}

void AudioSystem::ChannelGroup::SetVolume(float newVolume) noexcept {
    _groupVoice->SetVolume(newVolume);
}

float AudioSystem::ChannelGroup::GetVolume() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    float v{0.0f};
    _groupVoice->GetVolume(&v);
    return v;
}

void AudioSystem::ChannelGroup::Stop() noexcept {
    const auto& opId = _audio_system.IncrementAndGetOperationSetId();
    for(auto* channel : channels) {
        channel->Stop(opId);
    }
    _audio_system.SubmitDeferredOperation(opId);
}
