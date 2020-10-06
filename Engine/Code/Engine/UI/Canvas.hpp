#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/UI/Panel.hpp"


class Texture;
class Renderer;

namespace UI {

class CanvasSlot : public PanelSlot {
public:
    explicit CanvasSlot(Element* content = nullptr,
                        Panel* parent = nullptr);
    explicit CanvasSlot(const XMLElement& elem,
                        Element* content = nullptr,
                        Panel* parent = nullptr);
    void LoadFromXml(const XMLElement& elem);
    AABB2 anchors{};
    Vector2 position{};
    Vector2 size{};
    Vector2 alignment{};
    int zOrder{};
    bool autoSize{false};
    void CalcPivot() override;
    [[nodiscard]] Vector2 CalcPosition() const override;
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
    [[nodiscard]] const Camera2D& GetUICamera() const;

    void UpdateChildren(TimeUtils::FPSeconds deltaSeconds) override;
    void RenderChildren(Renderer& renderer) const override;

    [[nodiscard]] const Renderer& GetRenderer() const;
    [[nodiscard]] Renderer& GetRenderer();

    [[nodiscard]] static Vector4 AnchorTextToAnchorValues(const std::string& text) noexcept;

    CanvasSlot* AddChild(Element* child) override;
    CanvasSlot* AddChildAt(Element* child, std::size_t index) override;

    CanvasSlot* AddChildFromXml(const XMLElement& elem, Element* child) override;
    CanvasSlot* AddChildFromXml(const XMLElement& elem, Element* child, std::size_t index) override;

    void RemoveChild(Element* child) override;
    void RemoveAllChildren() override;

    [[nodiscard]] Vector4 CalcDesiredSize() const noexcept override;

protected:
    [[nodiscard]] AABB2 CalcChildrenDesiredBounds() const override;
    void ArrangeChildren() noexcept override;

private:
    void ReorderAllChildren();

    [[nodiscard]] bool LoadFromXml(const XMLElement& elem) noexcept;
    [[nodiscard]] std::pair<Vector2, float> CalcDimensionsAndAspectRatio() const;
    [[nodiscard]] AABB2 CalcAlignedAbsoluteBounds() const noexcept;

    mutable Camera2D _camera{};
    Renderer& _renderer;

    friend class CanvasSlot;
    friend class Widget;
};

} // namespace UI