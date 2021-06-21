#pragma once

/************************************************/
/* Audio System built based on HUGS system      */
/* by Youtube User ChiliTomatoNoodle            */
/* https://www.youtube.com/watch?v=T51Eqbbald4  */
/************************************************/

#include "Engine/Audio/Wav.hpp"
#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Win.hpp"

#include <Xaudio2.h>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <utility>
#include <vector>
#include <x3daudio.h>

#pragma comment(lib, "Xaudio2.lib")

namespace FileUtils {
class Wav;
}

class FileLogger;

class AudioSystem : public EngineSubsystem {
private:
    class Channel;
    class ChannelGroup;

public:
    class EngineCallback : public IXAudio2EngineCallback {
    public:
        virtual ~EngineCallback() {
        }
        virtual void STDMETHODCALLTYPE OnProcessingPassStart() override{};
        virtual void STDMETHODCALLTYPE OnProcessingPassEnd() override{};
        virtual void STDMETHODCALLTYPE OnCriticalError(HRESULT error) override;
    };
    class Sound {
    public:
        Sound(AudioSystem& audiosystem, std::filesystem::path filepath);
        void AddChannel(Channel* channel) noexcept;
        void RemoveChannel(Channel* channel) noexcept;
        [[nodiscard]] const std::size_t GetId() const noexcept;
        [[nodiscard]] static const std::size_t GetCount() noexcept;
        [[nodiscard]] const FileUtils::Wav* const GetWav() const noexcept;
        const std::vector<Channel*>& GetChannels() const noexcept;
    private:
        inline static std::size_t _id{0u};
        AudioSystem* _audio_system{};
        std::size_t _my_id{0u};
        FileUtils::Wav* _wave_file{};
        std::vector<Channel*> _channels{};
        mutable std::mutex _cs{};
    };
    struct SoundDesc {
        float volume{1.0f};
        float frequency{1.0f};
        int loopCount{0};
        bool stopWhenFinishedLooping{false};
        TimeUtils::FPSeconds loopBegin{};
        TimeUtils::FPSeconds loopEnd{};
        std::string groupName{};
    };

private:
    class Channel {
    public:
        class VoiceCallback : public IXAudio2VoiceCallback {
        public:
            virtual ~VoiceCallback() {
            }
            virtual void STDMETHODCALLTYPE OnVoiceProcessingPassStart(uint32_t /*bytesRequired*/) override{};
            virtual void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override{};
            virtual void STDMETHODCALLTYPE OnStreamEnd() override{};
            virtual void STDMETHODCALLTYPE OnBufferStart(void* /*pBufferContext*/) override{};
            virtual void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;
            virtual void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override;
            virtual void STDMETHODCALLTYPE OnVoiceError(void* /*pBufferContext*/, HRESULT /*Error*/) override{};
        };
        struct ChannelDesc {
            ChannelDesc() = default;
            ChannelDesc(const ChannelDesc& other) = default;
            ChannelDesc& operator=(const ChannelDesc& other) = default;
            ChannelDesc& operator=(const SoundDesc& sndDesc);
            explicit ChannelDesc(AudioSystem* audioSystem);

            AudioSystem* audio_system{nullptr};
            unsigned long long repeat_count{0ull};
            float volume{1.0f};
            float frequency{1.0f};
            float frequency_max{2.0f};
            uint32_t loop_count{0};
            uint32_t loop_beginSamples{0};
            uint32_t loop_endSamples{0};
            bool stopWhenFinishedLooping{false};
            std::string groupName{};
        };
        explicit Channel(AudioSystem& audioSystem, const ChannelDesc& desc) noexcept;
        ~Channel() noexcept;
        void Play(Sound& snd) noexcept;
        void Stop() noexcept;
        void Pause() noexcept;
        void SetStopWhenFinishedLooping(bool value);

        void Play(Sound& snd, uint32_t operationSetId) noexcept;
        void Stop(uint32_t operationSetId) noexcept;
        void Pause(uint32_t operationSetId) noexcept;

        void SetLoopCount(int count) noexcept;
        [[nodiscard]] uint32_t GetLoopCount() const noexcept;

        void SetLoopBegin(TimeUtils::FPSeconds start);
        void SetLoopEnd(TimeUtils::FPSeconds end);
        void SetLoopRange(TimeUtils::FPSeconds start, TimeUtils::FPSeconds end);

        [[nodiscard]] float GetVolume() const noexcept;
        void SetVolume(float newVolume) noexcept;

        [[nodiscard]] float GetFrequency() const noexcept;
        void SetFrequency(float newFrequency) noexcept;

    private:
        XAUDIO2_BUFFER _buffer{};
        IXAudio2SourceVoice* _voice = nullptr;
        Sound* _sound = nullptr;
        AudioSystem* _audio_system = nullptr;
        ChannelDesc _desc{};
        mutable std::mutex _cs{};

        friend class VoiceCallback;
        friend class ChannelGroup;
    };
    class ChannelGroup {
    public:
        ChannelGroup() = delete;
        explicit ChannelGroup(AudioSystem& parentAudioSystem) noexcept;
        explicit ChannelGroup(AudioSystem& parentAudioSystem, std::string name) noexcept;

        std::vector<Channel*> channels{};

        void AddChannel(Channel* channel) noexcept;
        void RemoveChannel(Channel* channel) noexcept;
        void SetVolume(float newVolume) noexcept;
        [[nodiscard]] float GetVolume() const noexcept;
        void Stop() noexcept;
    private:
        AudioSystem& _audio_system;
        std::string _name{"UNNAMED CHANNEL GROUP"};
        IXAudio2SubmixVoice* _groupVoice{};
        mutable std::mutex _cs{};
    };

public:
    explicit AudioSystem(FileLogger& fileLogger, std::size_t max_channels = 1024);
    AudioSystem(const AudioSystem& other) = delete;
    AudioSystem(AudioSystem&& other) = delete;
    AudioSystem& operator=(const AudioSystem& rhs) = delete;
    AudioSystem& operator=(AudioSystem&& rhs) = delete;
    virtual ~AudioSystem() noexcept;
    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update([[maybe_unused]] TimeUtils::FPSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    [[nodiscard]] virtual bool ProcessSystemMessage(const EngineMessage& msg) noexcept override;

    void SuspendAudio() noexcept;
    void ResumeAudio() noexcept;

    void SetFormat(const WAVEFORMATEXTENSIBLE& format) noexcept;
    void SetFormat(const FileUtils::Wav::WavFormatChunk& format) noexcept;

    void RegisterWavFilesFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept;
    void RegisterWavFile(std::filesystem::path filepath) noexcept;

    void Play(Sound& snd, SoundDesc desc = SoundDesc{}) noexcept;
    void Play(std::filesystem::path filepath, SoundDesc desc = SoundDesc{}) noexcept;
    [[nodiscard]] Sound* CreateSound(std::filesystem::path filepath) noexcept;
    [[nodiscard]] Sound* CreateSoundInstance(std::filesystem::path filepath) noexcept;

    [[nodiscard]] ChannelGroup* GetChannelGroup(const std::string& name) const noexcept;

    void SubmitDeferredOperation(uint32_t operationSetId) noexcept;
    const std::atomic_uint32_t& GetOperationSetId() const noexcept;
    const std::atomic_uint32_t& IncrementAndGetOperationSetId() noexcept;
    void IncrementOperationSetId() noexcept;

    void AddChannelGroup(const std::string& name) noexcept;
    void RemoveChannelGroup(const std::string& name) noexcept;
    void AddSoundToChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept;
    void AddSoundToChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept;
    void RemoveSoundFromChannelGroup(const std::string& channelGroupName, Sound* snd) noexcept;
    void RemoveSoundFromChannelGroup(const std::string& channelGroupName, const std::filesystem::path& filepath) noexcept;

    void SetEngineCallback(EngineCallback* callback) noexcept;
    [[nodiscard]] const WAVEFORMATEXTENSIBLE& GetFormat() const noexcept;
    [[nodiscard]] FileUtils::Wav::WavFormatChunk GetLoadedWavFileFormat() const noexcept;

protected:
private:
    void DeactivateChannel(Channel& channel) noexcept;
    FileLogger* _fileLogger = nullptr;
    WAVEFORMATEXTENSIBLE _audio_format_ex{};
    std::size_t _sound_count{};
    std::size_t _max_channels{};
    std::vector<std::pair<std::filesystem::path, std::unique_ptr<FileUtils::Wav>>> _wave_files{};
    std::vector<std::pair<std::filesystem::path, std::unique_ptr<Sound>>> _sounds{};
    std::vector<std::pair<std::filesystem::path, std::unique_ptr<ChannelGroup>>> _channel_groups{};
    std::vector<std::unique_ptr<Channel>> _active_channels{};
    std::vector<std::unique_ptr<Channel>> _idle_channels{};
    std::atomic_uint32_t _operationID{};
    IXAudio2* _xaudio2 = nullptr;
    X3DAUDIO_HANDLE _x3daudio;
    IXAudio2MasteringVoice* _master_voice = nullptr;
    EngineCallback _engine_callback{};
    std::mutex _cs{};
};
