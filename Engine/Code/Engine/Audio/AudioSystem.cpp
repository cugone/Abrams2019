#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Audio/Wav.hpp"

#include "Engine/Input/InputSystem.hpp"

#include <algorithm>

AudioSystem::AudioSystem(std::size_t max_channels /*= 1024*/)
    : EngineSubsystem()
    , _max_channels(max_channels)
{
    bool co_init_succeeded = SUCCEEDED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    GUARANTEE_OR_DIE(co_init_succeeded, "Failed to setup Audio System.");
    bool xaudio2_create_succeeded = SUCCEEDED(::XAudio2Create(&_xaudio2));
    GUARANTEE_OR_DIE(xaudio2_create_succeeded, "Failed to create Audio System.");
}

AudioSystem::~AudioSystem() noexcept {

    {
    std::scoped_lock<std::mutex> _lock(_cs);
    for(auto& channel : _active_channels) {
        channel->Stop();
    }
    }
    {
        bool done_cleanup = false;
        do {
            std::this_thread::yield();
            std::scoped_lock<std::mutex> _lock(_cs);
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

AudioSystem::ChannelGroup* AudioSystem::GetChannelGroup(const std::string& name) noexcept {
    auto found = _channel_groups.find(name);
    if(found != std::end(_channel_groups)) {
        return found->second.get();
    }
    return nullptr;
}

void AudioSystem::AddChannelGroup(const std::string& name) noexcept {
    auto group = std::make_unique<ChannelGroup>();
    auto found = _channel_groups.find(name);
    if(found != std::end(_channel_groups)) {
        found->second.reset(nullptr);
    }
    _channel_groups.insert_or_assign(name, std::move(group));
}

void AudioSystem::RemoveChannelGroup(const std::string& name) noexcept {
    auto found = _channel_groups.find(name);
    if(found != std::end(_channel_groups)) {
        found->second.reset(nullptr);
        _channel_groups.erase(found);
    }
}

void AudioSystem::AddSoundToChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept {
    if(!snd) {
        return;
    }
    if(auto group = GetChannelGroup(channelGroupName)) {
        if(group->channel) {
            group->sounds.push_back(snd);
        }
    }
}

void AudioSystem::AddSoundToChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept {
    if(auto group = GetChannelGroup(channelGroupName)) {
        if(group->channel) {
            auto* snd = CreateSound(filepath);
            group->sounds.push_back(snd);
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

void AudioSystem::Update([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) {
    /* DO NOTHING */
}

void AudioSystem::Render() const {
    /* DO NOTHING */
}

void AudioSystem::EndFrame() {
    std::scoped_lock<std::mutex> _lock(_cs);
    _idle_channels.erase(std::remove_if(std::begin(_idle_channels), std::end(_idle_channels), [](const std::unique_ptr<Channel>& c) { return c == nullptr; }), std::end(_idle_channels));
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
        std::ostringstream ss{};
        ss << "Attempting to Register Wav Files from unknown path: " << FS::absolute(folderpath) << std::endl;
        DebuggerPrintf(ss.str().c_str());
        return;
    }
    folderpath = FS::canonical(folderpath);
    folderpath.make_preferred();
    bool is_folder = FS::is_directory(folderpath);
    if(!is_folder) {
        return;
    }
    auto cb =
    [this](const std::filesystem::path& p) {
        RegisterWavFile(p);
    };
    FileUtils::ForEachFileInFolder(folderpath, ".wav", cb, recursive);
}

void AudioSystem::DeactivateChannel(Channel& channel) noexcept {
    std::scoped_lock<std::mutex> _lock(_cs);
    auto found_iter = std::find_if(std::begin(_active_channels), std::end(_active_channels),
                                   [&channel](const std::unique_ptr<Channel>& c) { return c.get() == &channel; });
    _idle_channels.push_back(std::move(*found_iter));
    _active_channels.erase(found_iter);
}

void AudioSystem::Play(Sound& snd) noexcept {
    std::scoped_lock<std::mutex> _lock(_cs);
    if(_max_channels <= _idle_channels.size()) {
        return;
    }
    if(_max_channels <= _active_channels.size()) {
        return;
    }
    _idle_channels.push_back(std::make_unique<Channel>(*this));
    _active_channels.push_back(std::move(_idle_channels.back()));
    _active_channels.back()->Play(snd);
}

void AudioSystem::Play(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        return;
    }
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    auto found_iter = _sounds.find(filepath);
    if(found_iter == _sounds.end()) {
        _sounds.insert_or_assign(filepath, std::move(std::make_unique<Sound>(*this, filepath)));
        found_iter = _sounds.find(filepath);
    }
    Sound* snd = found_iter->second.get();
    Play(*snd);
}

AudioSystem::Sound* AudioSystem::CreateSound(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        std::ostringstream msg;
        msg << "Could not find file: " << filepath << ".\n";
        DebuggerPrintf(msg.str().c_str());
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
        std::ostringstream msg;
        msg << "Attempting to register wav file that does not exist:";
        msg << '\n' << filepath;
        DebuggerPrintf(msg.str().c_str());
        return;
    }
    filepath = FS::canonical(filepath);
    auto found_iter = _wave_files.find(filepath);
    if(found_iter != _wave_files.end()) {
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
        case FileUtils::Wav::WAV_ERROR_NOT_A_WAV:
        {
            std::string e = filepath.string() + " is not a .wav file.\n";
            DebuggerPrintf(e.c_str());
            break;
        }
        case FileUtils::Wav::WAV_ERROR_BAD_FILE:
        {
            std::string e = filepath.string() + " is improperly formatted.\n";
            DebuggerPrintf(e.c_str());
            break;
        }
        default:
        {
            std::string e = "Unknown error attempting to load " + filepath.string() + "\n";
            DebuggerPrintf(e.c_str());
            break;
        }
    }
}

void STDMETHODCALLTYPE AudioSystem::Channel::VoiceCallback::OnBufferEnd(void* pBufferContext) {
    Channel& channel = *reinterpret_cast<Channel*>(pBufferContext);
    channel.Stop();
    channel._sound->RemoveChannel(&channel);
    channel._sound = nullptr;
    channel._audio_system->DeactivateChannel(channel);
}

AudioSystem::Channel::Channel(AudioSystem& audioSystem) noexcept
: _audio_system(&audioSystem)
{
    static VoiceCallback vcb;
    _buffer.pContext = this;
    auto fmt = reinterpret_cast<const WAVEFORMATEX*>(&(_audio_system->GetFormat()));
    _audio_system->_xaudio2->CreateSourceVoice(&_voice, fmt, 0, 2.0f, &vcb);
}

AudioSystem::Channel::~Channel() noexcept {
    if(_voice) {
        Stop();
        {
            std::scoped_lock<std::mutex> _lock(_cs);
            _voice->DestroyVoice();
            _voice = nullptr;
        }
    }
}

void AudioSystem::Channel::Play(Sound& snd) noexcept {
    snd.AddChannel(this);
    _sound = &snd;
    if(auto* wav = snd.GetWav()) {
        _buffer.pAudioData = wav->GetDataBuffer();
        _buffer.AudioBytes = wav->GetDataBufferSize();
        {
            std::scoped_lock<std::mutex> _lock(_cs);
            _voice->SubmitSourceBuffer(&_buffer, nullptr);
            _voice->Start();
        }
    }
}

void AudioSystem::Channel::Stop() noexcept {
    if(_voice && _sound) {
        std::scoped_lock<std::mutex> _lock(_cs);
        _voice->Stop();
        _voice->FlushSourceBuffers();
    }
}

void AudioSystem::Channel::SetVolume(float newVolume) noexcept {
    if(_voice) {
        std::scoped_lock<std::mutex> _lock(_cs);
        _voice->SetVolume(newVolume);
    }
}

std::size_t AudioSystem::Sound::_id = 0;

AudioSystem::Sound::Sound(AudioSystem& audiosystem, std::filesystem::path filepath)
    : _audio_system(&audiosystem)
{
    namespace FS = std::filesystem;
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
    std::scoped_lock<std::mutex> _lock(_cs);
    _channels.push_back(channel);
}

void AudioSystem::Sound::RemoveChannel(Channel* channel) noexcept {
    std::scoped_lock<std::mutex> _lock(_cs);
    _channels.erase(std::remove_if(std::begin(_channels), std::end(_channels),
                                   [channel](Channel* c)->bool { return c == channel; })
                    , std::end(_channels));
}

const std::size_t AudioSystem::Sound::GetId() const noexcept {
    return _my_id;
}

const std::size_t AudioSystem::Sound::GetCount() const noexcept {
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
