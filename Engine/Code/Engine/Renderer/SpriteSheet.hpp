#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"

#include <string>

class Renderer;
class Texture;

class SpriteSheet {
public:
    SpriteSheet(Renderer& renderer, const XMLElement& elem);
    ~SpriteSheet() = default;

    AABB2 GetTexCoordsFromSpriteCoords(int spriteX, int spriteY) const;
    AABB2 GetTexCoordsFromSpriteCoords(const IntVector2& spriteCoords) const;
    AABB2 GetTexCoordsFromSpriteIndex(int spriteIndex) const;
    int GetNumSprites() const;
    int GetFrameWidth() const;
    int GetFrameHeight() const;
    IntVector2 GetFrameDimensions() const;
    const IntVector2& GetLayout() const;
    const Texture& GetTexture() const;
    const Texture* GetTexture();
protected:
private:
    SpriteSheet(const Texture* texture, int tilesWide, int tilesHigh);
    SpriteSheet(Renderer& renderer, const std::string& texturePath, int tilesWide, int tilesHigh);

    void LoadFromXml(Renderer& renderer, const XMLElement& elem);
    const Texture* _spriteSheetTexture = nullptr;
    IntVector2 _spriteLayout{1, 1};

    friend class Renderer;
};