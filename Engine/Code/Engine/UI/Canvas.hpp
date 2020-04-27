#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/UI/Panel.hpp"


class Texture;
class Renderer;

namespace UI {

class CanvasSlot : public PanelSlot {
public:
    CanvasSlot() = default;
    explicit CanvasSlot(const XMLElement& elem);
    void LoadFromXml(const XMLElement& elem);
    AABB2 anchors{};
    Vector2 position{};
    Vector2 size{};
    Vector2 alignment{};
    int zOrder{};
    bool autoSize{false};
    void CalcPivot() override;
};

class Canvas : public Panel {
public:
    explicit Canvas(Widget* owner, Renderer& renderer);
    explicit Canvas(Widget* owner, Renderer& renderer, const XMLElement& elem);
    virtual ~Canvas() = default;
    void Update(TimeUtils::FPSeconds deltaSeconds) override;
    void Render(Renderer& renderer) const override;
    void SetupMVPFromTargetAndCamera(Renderer& renderer) const;
    void SetupMVPFromViewportAndCamera(Renderer& renderer) const;
    void DebugRender(Renderer& renderer) const override;
    void EndFrame() override;
    const Camera2D& GetUICamera() const;

    void UpdateChildren(TimeUtils::FPSeconds deltaSeconds) override;
    void RenderChildren(Renderer& renderer) const override;

    const Renderer& GetRenderer() const;
    Renderer& GetRenderer();

    static Vector4 AnchorTextToAnchorValues(const std::string& text) noexcept;

    CanvasSlot* AddChild(Element* child) override;
    CanvasSlot* AddChildAt(Element* child, std::size_t index) override;

    void RemoveChild(Element* child) override;
    void RemoveAllChildren() override;

    Vector4 CalcDesiredSize() const noexcept override;

protected:
    AABB2 CalcChildrenDesiredBounds() override;
    void ArrangeChildren() noexcept override;

private:
    void ReorderAllChildren();

    bool LoadFromXml(const XMLElement& elem) noexcept;
    std::pair<Vector2, float> CalcDimensionsAndAspectRatio() const;
    AABB2 CalcAlignedAbsoluteBounds() const noexcept;

    mutable Camera2D _camera{};
    Renderer& _renderer;

    friend class CanvasSlot;
    friend class Widget;
};

} // namespace UI