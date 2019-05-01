#pragma once

#include "Engine/UI/Element.hpp"

namespace UI {

class Canvas;

class Panel : public UI::Element {
public:
    explicit Panel(UI::Canvas* parent_canvas);
    virtual ~Panel() = default;

    virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render(Renderer* renderer) const override;
    virtual void DebugRender(Renderer* renderer, bool showSortOrder = false) const override;
protected:
private:
};

} //End UI