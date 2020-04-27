#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/UI/Element.hpp"

#include <memory>

class AnimatedSprite;

namespace UI {

class Panel;

class PictureBox : public Element {
public:
    explicit PictureBox(Panel* parent = nullptr);
    explicit PictureBox(const XMLElement& elem);
    virtual ~PictureBox() = default;

    void SetImage(std::unique_ptr<AnimatedSprite> sprite) noexcept;
    const AnimatedSprite* const GetImage() const noexcept;
    virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render(Renderer& renderer) const override;
    virtual void DebugRender(Renderer& renderer) const override;

    Vector4 CalcDesiredSize() const noexcept override;

protected:
private:
    bool LoadFromXml(const XMLElement& elem) noexcept;

    std::unique_ptr<AnimatedSprite> _sprite{};
};

} // namespace UI