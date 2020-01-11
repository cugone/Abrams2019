#pragma once

#include "Engine/UI/Element.hpp"

class AnimatedSprite;

namespace UI {

class Sprite : public UI::Element {
public:
	explicit Sprite(Canvas* parent_canvas, AnimatedSprite* sprite);
    virtual ~Sprite();

    virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render(Renderer& renderer) const override;
    virtual void DebugRender(Renderer& renderer, bool showSortOrder = false) const override;
protected:
private:
    AnimatedSprite* _sprite = nullptr;
};

} //End UI