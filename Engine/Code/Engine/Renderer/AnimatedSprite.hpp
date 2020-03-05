#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

class Material;
class Renderer;
class Texture;

class AnimatedSprite {
public:
    enum class SpriteAnimMode : int {
        Play_To_End,       // Play from time=0 to durationSeconds, then finish
        Play_To_Beginning, // Play from time=durationSeconds to 0, then finish
        Looping,           // Play from time=0 to end then repeat (never finish)
        Looping_Reverse,   // Play from time=durationSeconds then repeat (never finish)
        Ping_Pong,         // Play forwards, backwards, forwards, backwards...
        Max,
    };

    AnimatedSprite(Renderer& renderer, const XMLElement& elem) noexcept;
    AnimatedSprite(Renderer& renderer, std::weak_ptr<SpriteSheet> sheet, const XMLElement& elem) noexcept;
    AnimatedSprite(Renderer& renderer, std::weak_ptr<SpriteSheet> sheet, const IntVector2& startSpriteCoords) noexcept;
    ~AnimatedSprite() noexcept;

    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    AABB2 GetCurrentTexCoords() const noexcept;
    const Texture* const GetTexture() const noexcept;
    int GetNumSprites() const noexcept;
    IntVector2 GetFrameDimensions() const noexcept;
    void TogglePause() noexcept;
    void Pause() noexcept;                                    // Starts unpaused (playing) by default
    void Resume() noexcept;                                   // Resume after pausing
    void Reset() noexcept;                                    // Rewinds to time 0 and starts (re)playing
    bool IsFinished() const noexcept;                         //{ return m_isFinished; }
    bool IsPlaying() const noexcept;                          //{ return m_isPlaying; }
    TimeUtils::FPSeconds GetDurationSeconds() const noexcept; //{ return m_durationSeconds; }
    TimeUtils::FPSeconds GetSecondsElapsed() const noexcept;  //{ return m_elapsedSeconds; }
    TimeUtils::FPSeconds GetSecondsRemaining() const noexcept;
    float GetFractionElapsed() const noexcept;
    float GetFractionRemaining() const noexcept;
    void SetSecondsElapsed(TimeUtils::FPSeconds secondsElapsed) noexcept; // Jump to specific time
    void SetFractionElapsed(float fractionElapsed) noexcept;              // e.g. 0.33f for one-third in
    void SetMaterial(Material* mat) noexcept;
    Material* GetMaterial() const noexcept;

protected:
private:
    AnimatedSprite(Renderer& renderer, std::weak_ptr<SpriteSheet> spriteSheet, TimeUtils::FPSeconds durationSeconds, int startSpriteIndex, int frameLength, SpriteAnimMode playbackMode = SpriteAnimMode::Looping) noexcept;
    AnimatedSprite(Renderer& renderer, std::weak_ptr<SpriteSheet> spriteSheet, TimeUtils::FPSeconds durationSeconds, const IntVector2& startSpriteCoords, int frameLength, SpriteAnimMode playbackMode = SpriteAnimMode::Looping) noexcept;

    void LoadFromXml(Renderer& renderer, const XMLElement& elem) noexcept;
    SpriteAnimMode GetAnimModeFromOptions(bool looping, bool backwards, bool ping_pong /*= false*/) noexcept;
    int GetIndexFromCoords(const IntVector2& coords) noexcept;

    Renderer& _renderer;
    Material* _material = nullptr;
    std::weak_ptr<SpriteSheet> _sheet{};
    TimeUtils::FPSeconds _duration_seconds = TimeUtils::FPFrames{1};
    TimeUtils::FPSeconds _elapsed_seconds{0.0f};
    TimeUtils::FPSeconds _elapsed_frame_delta_seconds{0.0f};
    TimeUtils::FPSeconds _max_seconds_per_frame{0.0f};
    SpriteAnimMode _playback_mode = SpriteAnimMode::Looping;
    int _start_index{0};
    int _end_index{1};
    bool _is_playing = true;

    friend class Renderer;
};