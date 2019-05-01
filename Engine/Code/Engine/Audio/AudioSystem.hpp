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
        Sound(AudioSystem& audiosystem, const std::string& filepath);
        void AddChannel(Channel* channel);
        void RemoveChannel(Channel* channel);
        const std::size_t GetId() const;
        const std::size_t GetCount() const;
        const FileUtils::Wav* const GetWav() const;
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
        explicit Channel(AudioSystem& audioSystem);
        ~Channel();
        void Play(Sound& snd);
        void Stop();
        void SetVolume(float newVolume);
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
        void SetVolume(float newVolume);
    };
public:
    explicit AudioSystem(std::size_t max_channels = 1024);
    virtual ~AudioSystem();
    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update([[maybe_unused]]TimeUtils::FPSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

    void SetFormat(const WAVEFORMATEXTENSIBLE& format);
    void SetFormat(const FileUtils::Wav::WavFormatChunk& format);

    void RegisterWavFilesFromFolder(const std::string& folderpath, bool recursive = false);
    void RegisterWavFile(const std::string& filepath);

    void Play(Sound& snd);
    void Play(const std::string& filepath);
    Sound* CreateSound(const std::string& filepath);

    ChannelGroup* GetChannelGroup(const std::string& name);
    void AddChannelGroup(const std::string& name);
    void RemoveChannelGroup(const std::string& name);
    void AddSoundToChannelGroup(const std::string& channelGroupName, Sound* snd);
    void AddSoundToChannelGroup(const std::string& channelGroupName, const std::string& filepath);

    void SetEngineCallback(EngineCallback* callback);
    const WAVEFORMATEXTENSIBLE& GetFormat() const;
    FileUtils::Wav::WavFormatChunk GetLoadedWavFileFormat() const;
protected:
private:
    void DeactivateChannel(Channel& channel);
    void Play(const std::filesystem::path& filepath);
    Sound* CreateSound(const std::filesystem::path& filepath);

    void RegisterWavFilesFromFolder(const std::filesystem::path& folderpath, bool recursive = false);
    void RegisterWavFile(const std::filesystem::path& filepath);
    WAVEFORMATEXTENSIBLE _audio_format_ex{};
    std::size_t _sound_count{};
    std::size_t _max_channels{};
    std::map<std::string, std::unique_ptr<FileUtils::Wav>> _wave_files{};
    std::map<std::string, std::unique_ptr<Sound>> _sounds{};
    std::map<std::string, std::unique_ptr<ChannelGroup>> _channel_groups{};
    std::vector<std::unique_ptr<Channel>> _active_channels{};
    std::vector<std::unique_ptr<Channel>> _idle_channels{};
    IXAudio2* _xaudio2 = nullptr;
    IXAudio2MasteringVoice* _master_voice = nullptr;
    EngineCallback _engine_callback{};
    std::mutex _cs{};
};