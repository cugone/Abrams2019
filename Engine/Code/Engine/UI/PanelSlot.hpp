#pragma once

#include "Engine/Math/AABB2.hpp"

namespace UI {

class Panel;
class Element;

struct PanelSlot {
    Element* content{nullptr};
    Panel* parent{nullptr};
    explicit PanelSlot(Element* content = nullptr, Panel* parent = nullptr) : content(content), parent(parent) {}
    virtual ~PanelSlot() = default;
    virtual void CalcPivot() = 0;
    virtual Vector2 CalcPosition() const = 0;
};

struct NullPanelSlot : public PanelSlot {
    virtual ~NullPanelSlot() = default;
    void CalcPivot() override {};
    Vector2 CalcPosition() const override { return {}; };
};

} // namespace UI
