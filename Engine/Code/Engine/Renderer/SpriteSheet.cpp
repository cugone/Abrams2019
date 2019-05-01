#include "Engine/Renderer/SpriteSheet.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"

SpriteSheet::SpriteSheet(Renderer& renderer, const XMLElement& elem)
{
    LoadFromXml(renderer, elem);
}

SpriteSheet::SpriteSheet(const Texture* texture, int tilesWide, int tilesHigh)
    : _spriteSheetTexture(texture)
    , _spriteLayout(tilesWide, tilesHigh)
{
    /* DO NOTHING */
}

SpriteSheet::SpriteSheet(Renderer& renderer, const std::string& texturePath, int tilesWide, int tilesHigh)
    : _spriteSheetTexture(renderer.CreateOrGetTexture(texturePath, IntVector3::XY_AXIS))
    , _spriteLayout(tilesWide, tilesHigh)
{

}

AABB2 SpriteSheet::GetTexCoordsFromSpriteCoords(int spriteX, int spriteY) const {
    Vector2 texCoords(1.0f / _spriteLayout.x, 1.0f / _spriteLayout.y);

    float epsilon = 0.10f * (1.0f / 2048.0f);

    auto mins = Vector2{ texCoords.x * spriteX, texCoords.y * spriteY };
    auto maxs = Vector2{ texCoords.x * (spriteX + 1), texCoords.y * (spriteY + 1) };

    mins += Vector2{ epsilon, epsilon };
    maxs -= Vector2{ epsilon, epsilon };

    return AABB2(mins, maxs);
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteCoords(const IntVector2& spriteCoords) const {
    return GetTexCoordsFromSpriteCoords(spriteCoords.x, spriteCoords.y);
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteIndex(int spriteIndex) const {
    int x = spriteIndex % _spriteLayout.x;
    int y = spriteIndex / _spriteLayout.x;
    return GetTexCoordsFromSpriteCoords(x, y);
}

int SpriteSheet::GetNumSprites() const {
    return _spriteLayout.x * _spriteLayout.y;
}

int SpriteSheet::GetFrameWidth() const {
    return ((*_spriteSheetTexture).GetDimensions().x / _spriteLayout.x);
}

int SpriteSheet::GetFrameHeight() const {
    return ((*_spriteSheetTexture).GetDimensions().y / _spriteLayout.y);
}

IntVector2 SpriteSheet::GetFrameDimensions() const {
    return IntVector2(GetFrameWidth(), GetFrameHeight());
}


const IntVector2& SpriteSheet::GetLayout() const {
    return _spriteLayout;
}

const Texture& SpriteSheet::GetTexture() const {
    return *_spriteSheetTexture;
}

const Texture* SpriteSheet::GetTexture() {
    return _spriteSheetTexture;
}

void SpriteSheet::LoadFromXml(Renderer& renderer, const XMLElement& elem) {
    namespace FS = std::filesystem;
    DataUtils::ValidateXmlElement(elem, "spritesheet", "", "src,dimensions");
    _spriteLayout = DataUtils::ParseXmlAttribute(elem, "dimensions", _spriteLayout);
    std::string texturePathAsString{};
    texturePathAsString = DataUtils::ParseXmlAttribute(elem, "src", texturePathAsString);
    FS::path p{ texturePathAsString };
    p = FS::canonical(p);
    p.make_preferred();
    _spriteSheetTexture = renderer.CreateOrGetTexture(p.string(), IntVector3::XY_AXIS);
}
