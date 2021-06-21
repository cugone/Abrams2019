#pragma once

#include <filesystem>

class IAudioService {
public:
    virtual ~IAudioService() { /* DO NOTHING */ };
    virtual void SuspendAudio() noexcept = 0;
    virtual void ResumeAudio() noexcept = 0;

    virtual void Play(const std::filesystem::path& filepath) noexcept = 0;
    virtual void Play(const std::size_t id) noexcept = 0;
    virtual void Play(const std::filesystem::path& filepath, const bool looping) noexcept = 0;
    virtual void Play(const std::size_t id, const bool looping) noexcept = 0;
    
    virtual void Stop(const std::filesystem::path& filepath) noexcept = 0;
    virtual void Stop(const std::size_t id) noexcept = 0;
    virtual void StopAll() noexcept = 0;
protected:
private:
};
