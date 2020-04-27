#pragma once

#include "Engine/Math/AABB2.hpp"

namespace UI {

class Panel;
class Element;

struct PanelSlot {
    Element* content{nullptr};
    Panel* parent{nullptr};
    virtual ~PanelSlot() = default;
    virtual void CalcPivot() = 0;
};

struct NullPanelSlot : public PanelSlot {
    virtual ~NullPanelSlot() = default;
    void CalcPivot() override {};
};

} // namespace UI
