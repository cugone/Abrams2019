#pragma once

/************************************************/
/* Audio System built based on HUGS system      */
/* by Youtube User ChiliTomatoNoodle            */
/* https://www.youtube.com/watch?v=T51Eqbbald4  */
/************************************************/

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Audio/Wav.hpp"

#include <iomanip>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>
#include <Xaudio2.h>

#pragma comment(lib, "Xaudio2.lib")

namespace FileUtils {
class Wav;
}

class AudioSystem : public EngineSubsystem {
private:
    class Channel;
public:
    class EngineCallback : public IXAudio2EngineCallback {
    public:
        virtual ~EngineCallback() {}
        virtual void STDMETHODCALLTYPE OnProcessingPassStart() override {};
        virtual void STDMETHODCALLTYPE OnProcessingPassEnd() override {};
        virtual void STDMETHODCALLTYPE OnCriticalError(HRESULT error) override;
    };
    class Sound {
    public:
        Sound(AudioSystem& audiosystem, std::filesystem::path filepath);
        void AddChannel(Channel* channel) noexcept;
        void RemoveChannel(Channel* channel) noexcept;
        const std::size_t GetId() const noexcept;
        const std::size_t GetCount() const noexcept;
        const FileUtils::Wav* const GetWav() const noexcept;
    private:
        static std::size_t _id;
        AudioSystem* _audio_system{};
        std::size_t _my_id = 0;
        FileUtils::Wav* _wave_file{};
        std::vector<Channel*> _channels{};
        std::mutex _cs{};
    };
private:
    class Channel {
    public:
        class VoiceCallback : public IXAudio2VoiceCallback {
        public:
            virtual ~VoiceCallback() {}
            virtual void STDMETHODCALLTYPE OnVoiceProcessingPassStart(uint32_t /*bytesRequired*/) override {};
            virtual void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {};
            virtual void STDMETHODCALLTYPE OnStreamEnd() override {};
            virtual void STDMETHODCALLTYPE OnBufferStart(void* /*pBufferContext*/) override {};
            virtual void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;
            virtual void STDMETHODCALLTYPE OnLoopEnd(void* /*pBufferContext*/) override {};
            virtual void STDMETHODCALLTYPE OnVoiceError(void* /*pBufferContext*/, HRESULT /*Error*/) override {};
        };
        explicit Channel(AudioSystem& audioSystem) noexcept;
        ~Channel() noexcept;
        void Play(Sound& snd) noexcept;
        void Stop() noexcept;
        void SetVolume(float newVolume) noexcept;
    private:
        XAUDIO2_BUFFER _buffer{};
        IXAudio2SourceVoice* _voice = nullptr;
        Sound* _sound = nullptr;
        AudioSystem* _audio_system = nullptr;
        std::mutex _cs{};
    };
    struct ChannelGroup {
        Channel* channel = nullptr;
        std::vector<Sound*> sounds{};
        void SetVolume(float newVolume) noexcept;
    };
public:
    explicit AudioSystem(std::size_t max_channels = 1024);
    AudioSystem(const AudioSystem& other) = delete;
    AudioSystem(AudioSystem&& other) = delete;
    AudioSystem& operator=(const AudioSystem& rhs) = delete;
    AudioSystem& operator=(AudioSystem&& rhs) = delete;
    virtual ~AudioSystem() noexcept;
    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update([[maybe_unused]]TimeUtils::FPSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) noexcept override;

    void SetFormat(const WAVEFORMATEXTENSIBLE& format) noexcept;
    void SetFormat(const FileUtils::Wav::WavFormatChunk& format) noexcept;

    void RegisterWavFilesFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept;
    void RegisterWavFile(std::filesystem::path filepath) noexcept;

    void Play(Sound& snd) noexcept;
    void Play(std::filesystem::path filepath) noexcept;
    Sound* CreateSound(std::filesystem::path filepath) noexcept;

    ChannelGroup* GetChannelGroup(const std::string& name) noexcept;
    void AddChannelGroup(const std::string& name) noexcept;
    void RemoveChannelGroup(const std::string& name) noexcept;
    void AddSoundToChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept;
    void AddSoundToChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept;

    void SetEngineCallback(EngineCallback* callback) noexcept;
    const WAVEFORMATEXTENSIBLE& GetFormat() const noexcept;
    FileUtils::Wav::WavFormatChunk GetLoadedWavFileFormat() const noexcept;
protected:
private:
    void DeactivateChannel(Channel& channel) noexcept;

    WAVEFORMATEXTENSIBLE _audio_format_ex{};
    std::size_t _sound_count{};
    std::size_t _max_channels{};
    std::map<std::filesystem::path, std::unique_ptr<FileUtils::Wav>> _wave_files{};
    std::map<std::filesystem::path, std::unique_ptr<Sound>> _sounds{};
    std::map<std::string, std::unique_ptr<ChannelGroup>> _channel_groups{};
    std::vector<std::unique_ptr<Channel>> _active_channels{};
    std::vector<std::unique_ptr<Channel>> _idle_channels{};
    IXAudio2* _xaudio2 = nullptr;
    IXAudio2MasteringVoice* _master_voice = nullptr;
    EngineCallback _engine_callback{};
    std::mutex _cs{};
};