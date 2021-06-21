#pragma once

#include <filesystem>

class IAudioService {
public:
    virtual ~IAudioService() { /* DO NOTHING */ };
    virtual void SuspendAudio() noexcept = 0;
    virtual void ResumeAudio() noexcept = 0;

    virtual void Play(const std::filesystem::path& filepath) = 0;
    virtual void Play(const std::size_t id) = 0;
    virtual void Play(const std::filesystem::path& filepath, const bool looping) = 0;
    virtual void Play(const std::size_t id, const bool looping) = 0;
    
    virtual void Stop(const std::filesystem::path& filepath) = 0;
    virtual void Stop(const std::size_t id) = 0;
    virtual void StopAll() = 0;
protected:
private:
};
