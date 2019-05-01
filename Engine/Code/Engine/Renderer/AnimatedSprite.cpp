#include "Engine/Renderer/AnimatedSprite.hpp"

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

AnimatedSprite::AnimatedSprite(Renderer& renderer,
                               SpriteSheet* spriteSheet,
                               TimeUtils::FPSeconds durationSeconds,
                               int startSpriteIndex,
                               int frameLength,
                               SpriteAnimMode playbackMode /*= SpriteAnimMode::LOOPING*/)
    : _renderer(&renderer)
    , _sheet(spriteSheet)
    , _duration_seconds(durationSeconds)
    , _playback_mode(playbackMode)
    , _start_index(startSpriteIndex)
    , _end_index(startSpriteIndex + frameLength)
{
    bool has_frames = frameLength > 0;
    _max_seconds_per_frame = TimeUtils::FPSeconds{ _duration_seconds / (has_frames ? static_cast<float>(_end_index - _start_index) : 1.0f) };
};

AnimatedSprite::AnimatedSprite(Renderer& renderer, const XMLElement& elem)
    : _renderer(&renderer)
{
    LoadFromXml(*_renderer, elem);
}

AnimatedSprite::AnimatedSprite(Renderer& renderer, SpriteSheet* sheet)
    : AnimatedSprite(renderer, sheet, TimeUtils::FPFrames{1}, 0, 0)
{
    /* DO NOTHING */
}

AnimatedSprite::~AnimatedSprite() {
    _renderer = nullptr;
    delete _sheet;
    _sheet = nullptr;
}

void AnimatedSprite::Update(TimeUtils::FPSeconds deltaSeconds) {

    _elapsed_frame_delta_seconds += deltaSeconds;
    while(_elapsed_frame_delta_seconds >= _max_seconds_per_frame) {
        _elapsed_frame_delta_seconds -= _max_seconds_per_frame;
    }
    switch(_playback_mode) {
        case SpriteAnimMode::Looping:
            if(IsFinished()) {
                while(_elapsed_seconds >= _duration_seconds) {
                    _elapsed_seconds -= _duration_seconds;
                }
            }
            break;
        case SpriteAnimMode::Looping_Reverse:
            if(IsFinished()) {
                _elapsed_seconds = _duration_seconds;
                deltaSeconds *= -1.0f;
            }
            break;
        case SpriteAnimMode::Play_To_Beginning:
            if(IsFinished()) {
                _is_playing = false;
                _elapsed_seconds = TimeUtils::FPSeconds{0.0f};
            }
            break;
        case SpriteAnimMode::Play_To_End:
            if(IsFinished()) {
                _is_playing = false;
                _elapsed_seconds = _duration_seconds;
            }
            break;
        case SpriteAnimMode::Ping_Pong:
            if(_elapsed_seconds < TimeUtils::FPSeconds{0.0f}) {
                deltaSeconds *= -1.0f;
                while(_elapsed_seconds < TimeUtils::FPSeconds{0.0f}) {
                    _elapsed_seconds += _duration_seconds;
                }
            } else if(_elapsed_seconds >= _duration_seconds) {
                deltaSeconds *= -1.0f;
                while(_elapsed_seconds >= _duration_seconds) {
                    _elapsed_seconds -= _duration_seconds;
                }
            }
            break;
        default:
            break;
    }
    _elapsed_seconds += deltaSeconds;
}

AABB2 AnimatedSprite::GetCurrentTexCoords() const {

    int length = _end_index - _start_index;
    auto framesPerSecond = TimeUtils::FPSeconds{ 1.0f } / _max_seconds_per_frame;
    auto frameIndex = static_cast<int>(_elapsed_seconds.count() * framesPerSecond);
    switch(_playback_mode) {
        case SpriteAnimMode::Play_To_End:
            if(frameIndex >= length) {
                frameIndex = _end_index - 1;
            }
            break;
        case SpriteAnimMode::Play_To_Beginning:
            if(frameIndex < 0) {
                frameIndex = 0;
            }
            break;
        case SpriteAnimMode::Looping: /* FALLTHROUGH */
        case SpriteAnimMode::Looping_Reverse:
            if(frameIndex >= length) {
                frameIndex = 0;
            }
            if(frameIndex < 0) {
                frameIndex = _end_index - 1;
            }
            break;
        case SpriteAnimMode::Ping_Pong:
            if(frameIndex >= length) {
                frameIndex = _end_index - 1;
            }
            if(frameIndex < 0) {
                frameIndex = 0;
            }
            break;
        default:
            break;
    }

    return _sheet->GetTexCoordsFromSpriteIndex(frameIndex);
}

const Texture* const AnimatedSprite::GetTexture() const {
    return _sheet->GetTexture();
}

int AnimatedSprite::GetNumSprites() const {
    return _sheet->GetNumSprites();
}

IntVector2 AnimatedSprite::GetFrameDimensions() const {
    return _sheet->GetFrameDimensions();
}

void AnimatedSprite::TogglePause() {
    _is_playing = !_is_playing;
}

void AnimatedSprite::Pause() {
    _is_playing = false;
}

void AnimatedSprite::Resume() {
    _is_playing = true;
}

void AnimatedSprite::Reset() {
    _elapsed_seconds = TimeUtils::FPSeconds{0.0f};
}

bool AnimatedSprite::IsFinished() const {
    if(!_is_playing) {
        return false;
    }
    switch(_playback_mode) {
        case SpriteAnimMode::Looping: /* FALL THROUGH */
        case SpriteAnimMode::Play_To_End:
            return !(_elapsed_seconds < _duration_seconds);
        case SpriteAnimMode::Looping_Reverse: /* FALL THROUGH */
        case SpriteAnimMode::Play_To_Beginning:
            return _elapsed_seconds < TimeUtils::FPSeconds{0.0f};
        case SpriteAnimMode::Ping_Pong:
            return false;
        default:
            return !(_elapsed_seconds < _duration_seconds);
    }
}

bool AnimatedSprite::IsPlaying() const {
    return _is_playing;
}

TimeUtils::FPSeconds AnimatedSprite::GetDurationSeconds() const {
    return _duration_seconds;
}

TimeUtils::FPSeconds AnimatedSprite::GetSecondsElapsed() const {
    return _elapsed_seconds;
}

TimeUtils::FPSeconds AnimatedSprite::GetSecondsRemaining() const {
    return _duration_seconds - _elapsed_seconds;
}

float AnimatedSprite::GetFractionElapsed() const {
    return _elapsed_seconds / _duration_seconds;
}

float AnimatedSprite::GetFractionRemaining() const {
    return (_duration_seconds - _elapsed_seconds) / _duration_seconds;
}

void AnimatedSprite::SetSecondsElapsed(TimeUtils::FPSeconds secondsElapsed) {
    _elapsed_seconds = secondsElapsed;
}

void AnimatedSprite::SetFractionElapsed(float fractionElapsed) {
    _elapsed_seconds = _duration_seconds * fractionElapsed;
}

void AnimatedSprite::SetMaterial(Material* mat) {
    _material = mat;
}

Material* AnimatedSprite::GetMaterial() const {
    return _material;
}

AnimatedSprite::SpriteAnimMode AnimatedSprite::GetAnimModeFromOptions(bool looping, bool backwards, bool ping_pong /*= false*/) {
    if(ping_pong) {
        return SpriteAnimMode::Ping_Pong;
    }

    if(looping) {
        if(backwards) {
            return SpriteAnimMode::Looping_Reverse;
        }
        return SpriteAnimMode::Looping;
    }
    if(backwards) {
        return SpriteAnimMode::Play_To_Beginning;
    }
    return SpriteAnimMode::Play_To_End;
}

void AnimatedSprite::LoadFromXml(Renderer& renderer, const XMLElement& elem) {
    DataUtils::ValidateXmlElement(elem, "animation", "spritesheet,animationset", "", "", "name");
    auto xml_sheet = elem.FirstChildElement("spritesheet");
    DataUtils::ValidateXmlElement(*xml_sheet, "spritesheet", "", "src,dimensions");
    _sheet = renderer.CreateSpriteSheet(*xml_sheet);

    auto xml_animset = elem.FirstChildElement("animationset");
    DataUtils::ValidateXmlElement(*xml_animset, "animationset", "", "startindex,framelength,duration", "", "loop,reverse,pingpong");

    int start_index = 0;
    _start_index = DataUtils::ParseXmlAttribute(*xml_animset, "startindex", start_index);
    auto frameLength = DataUtils::ParseXmlAttribute(*xml_animset, "framelength", 0);
    _end_index = _start_index + frameLength;

    TimeUtils::FPSeconds min_duration = TimeUtils::FPFrames{1};
    float duration_seconds = 0.0f;
    _duration_seconds = TimeUtils::FPSeconds{DataUtils::ParseXmlAttribute(*xml_animset, "duration", duration_seconds)};
    if(_duration_seconds < min_duration) {
        _duration_seconds = min_duration;
    }

    bool is_looping = DataUtils::ParseXmlAttribute(*xml_animset, "loop", false);
    bool is_reverse = DataUtils::ParseXmlAttribute(*xml_animset, "reverse", false);
    bool is_pingpong = DataUtils::ParseXmlAttribute(*xml_animset, "pingpong", false);
    _playback_mode = GetAnimModeFromOptions(is_looping, is_reverse, is_pingpong);

    bool has_frames = frameLength > 0;
    _max_seconds_per_frame = TimeUtils::FPSeconds{ _duration_seconds / (has_frames ? static_cast<float>(_end_index - _start_index) : 1.0f) };
}
