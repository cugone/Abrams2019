#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Audio/Wav.hpp"
#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Input/InputSystem.hpp"

#include <algorithm>

AudioSystem::AudioSystem(FileLogger& fileLogger, std::size_t max_channels /*= 1024*/)
: EngineSubsystem()
, _fileLogger(&fileLogger)
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
    if(const auto found = _channel_groups.find(name); found != std::end(_channel_groups)) {
        return found->second.get();
    }
    return nullptr;
}

AudioSystem::ChannelGroup* AudioSystem::GetChannelGroup(Sound* snd) const noexcept {
    for(const auto& group : _channel_groups) {
        for(const auto* sound : group.second->sounds) {
            if(snd == sound) {
                return group.second.get();
            }
        }
    }
    return nullptr;
}

void AudioSystem::AddChannelGroup(const std::string& name) noexcept {
    if(auto found = _channel_groups.find(name); found != std::end(_channel_groups)) {
        if(found->second->channel) {
            DeactivateChannel(*found->second->channel);
        }
        found->second.reset(nullptr);
    }
    auto group = std::make_unique<ChannelGroup>();
    auto channel = std::make_unique<Channel>(*this, AudioSystem::Channel::ChannelDesc{});
    auto* channel_ptr = channel.get();
    _idle_channels.push_back(std::move(channel));
    group->channel = channel_ptr;
    _channel_groups.insert_or_assign(name, std::move(group));
}

void AudioSystem::RemoveChannelGroup(const std::string& name) noexcept {
    if(auto found = _channel_groups.find(name); found != std::end(_channel_groups)) {
        if(found->second->channel) {
            DeactivateChannel(*found->second->channel);
        }
        found->second.reset(nullptr);
        _channel_groups.erase(found);
    }
}

void AudioSystem::AddSoundToChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept {
    if(!snd) {
        return;
    }
    if(auto group = GetChannelGroup(channelGroupName); group && group->channel) {
        group->sounds.push_back(snd);
        snd->AddChannel(group->channel);
    }
}

void AudioSystem::AddSoundToChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept {
    if(auto group = GetChannelGroup(channelGroupName); group && group->channel) {
        auto* snd = CreateSound(filepath);
        AddSoundToChannelGroup(channelGroupName, snd);
    }
}

void AudioSystem::RemoveSoundFromChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept {
    if(!snd) {
        return;
    }
    if(auto group = GetChannelGroup(channelGroupName); group && group->channel) {
        if(auto iter = std::find(std::begin(group->sounds), std::end(group->sounds), snd); iter != std::end(group->sounds)) {
            std::iter_swap(iter, group->sounds.end() - 1);
            group->sounds.pop_back();
            snd->RemoveChannel(group->channel);
        }
    }
}

void AudioSystem::RemoveSoundFromChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept {
    if(const auto group = GetChannelGroup(channelGroupName); group && group->channel) {
        if(const auto found_iter = _sounds.find(filepath); found_iter != std::end(_sounds)) {
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
    if(auto* group = GetChannelGroup(&snd)) {
        const auto [groupvolume, groupfrequency] = group->GetVolumeAndFrequency();
        if(desc.volume == 1.0f) {
            desc.volume = groupvolume;
        }
        if(desc.frequency == 1.0f) {
            desc.frequency = groupfrequency;
        }
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

AudioSystem::Sound* AudioSystem::CreateSound(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        DebuggerPrintf("Could not find file: %s\n", filepath.string().c_str());
        return nullptr;
    }

    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    auto filepathAsString = filepath.string();
    auto found_iter = _sounds.find(filepathAsString);
    if(found_iter == _sounds.end()) {
        _sounds.insert_or_assign(filepathAsString, std::move(std::make_unique<Sound>(*this, filepathAsString)));
        found_iter = _sounds.find(filepathAsString);
    }
    return found_iter->second.get();
}

void AudioSystem::RegisterWavFile(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        DebuggerPrintf("Attempting to register wav file that does not exist: %s\n", filepath.string().c_str());
        return;
    }
    filepath = FS::canonical(filepath);
    if(auto found_iter = _wave_files.find(filepath); found_iter != _wave_files.end()) {
        return;
    }

    unsigned int wav_result = FileUtils::Wav::WAV_SUCCESS;
    {
        auto wav = std::make_unique<FileUtils::Wav>();
        wav_result = wav->Load(filepath);
        if(wav_result == FileUtils::Wav::WAV_SUCCESS) {
            _wave_files.insert_or_assign(filepath, std::move(wav));
            return;
        }
    }
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

void AudioSystem::Channel::Stop() noexcept {
    if(_voice && _sound) {
        std::scoped_lock<std::mutex> lock(_cs);
        _voice->Stop();
        _voice->FlushSourceBuffers();
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
    auto found_iter = _audio_system->_wave_files.find(filepath);
    if(found_iter == _audio_system->_wave_files.end()) {
        _audio_system->RegisterWavFile(filepath);
        found_iter = _audio_system->_wave_files.find(filepath);
    }
    if(found_iter != _audio_system->_wave_files.end()) {
        _my_id = _id++;
        _wave_file = found_iter->second.get();
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

void STDMETHODCALLTYPE AudioSystem::EngineCallback::OnCriticalError(HRESULT error) {
    std::ostringstream ss;
    ss << "The Audio System encountered a fatal error: ";
    ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << error;
    ERROR_AND_DIE(ss.str().c_str());
}

void AudioSystem::ChannelGroup::SetVolume(float newVolume) noexcept {
    channel->SetVolume(newVolume);
}

void AudioSystem::ChannelGroup::SetFrequency(float newFrequency) noexcept {
    channel->SetFrequency(newFrequency);
}

constexpr std::pair<float, float> AudioSystem::ChannelGroup::GetVolumeAndFrequency() const noexcept {
    return std::make_pair(GetVolume(), GetFrequency());
}

float AudioSystem::ChannelGroup::GetFrequency() const noexcept {
    return channel->GetFrequency();
}

float AudioSystem::ChannelGroup::GetVolume() const noexcept {
    return channel->GetVolume();
}
