#include "Engine/Renderer/SpriteSheet.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <sstream>

SpriteSheet::SpriteSheet(Renderer& renderer, const XMLElement& elem) noexcept
{
    LoadFromXml(renderer, elem);
}

SpriteSheet::SpriteSheet(Texture* texture, int tilesWide, int tilesHigh) noexcept
    : _spriteSheetTexture(texture)
    , _spriteLayout(tilesWide, tilesHigh)
{
    /* DO NOTHING */
}

SpriteSheet::SpriteSheet(Renderer& renderer, const std::filesystem::path& texturePath, int tilesWide, int tilesHigh) noexcept
    : _spriteSheetTexture(renderer.CreateOrGetTexture(texturePath, IntVector3::XY_AXIS))
    , _spriteLayout(tilesWide, tilesHigh)
{
    /* DO NOTHING */
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteCoords(int spriteX, int spriteY) const noexcept {
    Vector2 texCoords(1.0f / _spriteLayout.x, 1.0f / _spriteLayout.y);

    float epsilon = 0.10f * (1.0f / 2048.0f);

    auto mins = Vector2{ texCoords.x * spriteX, texCoords.y * spriteY };
    auto maxs = Vector2{ texCoords.x * (spriteX + 1), texCoords.y * (spriteY + 1) };

    mins += Vector2{ epsilon, epsilon };
    maxs -= Vector2{ epsilon, epsilon };

    return AABB2(mins, maxs);
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteCoords(const IntVector2& spriteCoords) const noexcept {
    return GetTexCoordsFromSpriteCoords(spriteCoords.x, spriteCoords.y);
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteIndex(int spriteIndex) const noexcept {
    int x = spriteIndex % _spriteLayout.x;
    int y = spriteIndex / _spriteLayout.x;
    return GetTexCoordsFromSpriteCoords(x, y);
}

int SpriteSheet::GetNumSprites() const noexcept {
    return _spriteLayout.x * _spriteLayout.y;
}

int SpriteSheet::GetFrameWidth() const noexcept {
    return ((*_spriteSheetTexture).GetDimensions().x / _spriteLayout.x);
}

int SpriteSheet::GetFrameHeight() const noexcept {
    return ((*_spriteSheetTexture).GetDimensions().y / _spriteLayout.y);
}

IntVector2 SpriteSheet::GetFrameDimensions() const noexcept {
    return IntVector2(GetFrameWidth(), GetFrameHeight());
}


const IntVector2& SpriteSheet::GetLayout() const noexcept {
    return _spriteLayout;
}

const Texture* SpriteSheet::GetTexture() const noexcept {
    return _spriteSheetTexture;
}

Texture* SpriteSheet::GetTexture() noexcept {
    return _spriteSheetTexture;
}

void SpriteSheet::LoadFromXml(Renderer& renderer, const XMLElement& elem) noexcept {
    namespace FS = std::filesystem;
    DataUtils::ValidateXmlElement(elem, "spritesheet", "", "src,dimensions");
    _spriteLayout = DataUtils::ParseXmlAttribute(elem, "dimensions", _spriteLayout);
    std::string texturePathAsString{};
    texturePathAsString = DataUtils::ParseXmlAttribute(elem, "src", texturePathAsString);
    FS::path p{ texturePathAsString };
    {
        std::error_code ec{};
        p = FS::canonical(p, ec);
        if(ec) {
            auto ss = std::string{"Error loading spritesheet at "} +texturePathAsString + ":\n" + ec.message();
            ERROR_AND_DIE(ss.c_str());
        }
    }
    p.make_preferred();
    _spriteSheetTexture = renderer.CreateOrGetTexture(p.string(), IntVector3::XY_AXIS);
}
