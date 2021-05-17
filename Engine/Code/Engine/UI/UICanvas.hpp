#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/UI/UIPanel.hpp"

class Texture;
class Renderer;

class UICanvasSlot : public UIPanelSlot {
public:
    explicit UICanvasSlot(UIElement* content = nullptr,
                        UIPanel* parent = nullptr);
    explicit UICanvasSlot(const XMLElement& elem,
                        UIElement* content = nullptr,
                        UIPanel* parent = nullptr);
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

class UICanvas : public UIPanel {
public:
    explicit UICanvas(UIWidget* owner, Renderer& renderer);
    explicit UICanvas(UIWidget* owner, Renderer& renderer, const XMLElement& elem);
    virtual ~UICanvas() = default;
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

    UICanvasSlot* AddChild(UIElement* child) override;
    UICanvasSlot* AddChildAt(UIElement* child, std::size_t index) override;

    UICanvasSlot* AddChildFromXml(const XMLElement& elem, UIElement* child) override;
    UICanvasSlot* AddChildFromXml(const XMLElement& elem, UIElement* child, std::size_t index) override;

    void RemoveChild(UIElement* child) override;
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

    friend class UICanvasSlot;
    friend class UIWidget;
};
