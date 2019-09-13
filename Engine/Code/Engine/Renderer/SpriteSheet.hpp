#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"

#include <filesystem>
#include <string>

class Renderer;
class Texture;

class SpriteSheet {
public:
    SpriteSheet(Renderer& renderer, const XMLElement& elem) noexcept;
    ~SpriteSheet() = default;

    AABB2 GetTexCoordsFromSpriteCoords(int spriteX, int spriteY) const noexcept;
    AABB2 GetTexCoordsFromSpriteCoords(const IntVector2& spriteCoords) const noexcept;
    AABB2 GetTexCoordsFromSpriteIndex(int spriteIndex) const noexcept;
    int GetNumSprites() const noexcept;
    int GetFrameWidth() const noexcept;
    int GetFrameHeight() const noexcept;
    IntVector2 GetFrameDimensions() const noexcept;
    const IntVector2& GetLayout() const noexcept;
    const Texture* GetTexture() const noexcept;
    Texture* GetTexture() noexcept;
protected:
private:
    SpriteSheet(Texture* texture, int tilesWide, int tilesHigh) noexcept;
    SpriteSheet(Renderer& renderer, const std::filesystem::path& texturePath, int tilesWide, int tilesHigh) noexcept;

    void LoadFromXml(Renderer& renderer, const XMLElement& elem) noexcept;
    Texture* _spriteSheetTexture = nullptr;
    IntVector2 _spriteLayout{1, 1};

    friend class Renderer;
};