#pragma once

#include "Engine/Renderer/Camera2D.hpp"

#include "Engine/UI/Element.hpp"

class Texture;
class Renderer;

namespace UI {

class Canvas : public UI::Element {
public:
    explicit Canvas(Renderer& renderer, float reference_resolution, Texture* target_texture = nullptr, Texture* target_depthStencil = nullptr);
    virtual ~Canvas() = default;
    virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render(Renderer* renderer) const override;
    void SetupMVPFromTargetAndCamera(Renderer* renderer) const;
    virtual void DebugRender(Renderer* renderer, bool showSortOrder = false) const override;
    const Camera2D& GetUICamera() const;

    template<typename T>
    T* CreateChild();
    template<typename T, typename ...Args>
    T* CreateChild(Args&&... args);
    template<typename T>
    T* CreateChildBefore(UI::Element* youngerSibling);
    template<typename T, typename ...Args>
    T* CreateChildBefore(UI::Element* youngerSibling, Args&&... args);
    template<typename T>
    T* CreateChildAfter(UI::Element* olderSibling);
    template<typename T, typename ...Args>
    T* CreateChildAfter(UI::Element* olderSibling, Args&&... args);

protected:
private:
    void CalcDimensionsAndAspectRatio(Vector2& dimensions, float& aspectRatio);
    void SetTargetTexture(Renderer& renderer, Texture* target, Texture* depthstencil);

    mutable Camera2D _camera{};
    Renderer* _renderer = nullptr;
    Texture* _target_texture = nullptr;
    Texture* _target_depthstencil = nullptr;
    float _reference_resolution = 0.0f;
    float _aspect_ratio = 1.0f;
};

template<typename T>
T* UI::Canvas::CreateChild() {
    return dynamic_cast<T*>(Element::CreateChild<T>(this));
}

template<typename T, typename ...Args>
T* UI::Canvas::CreateChild(Args&&... args) {
    return dynamic_cast<T*>(Element::CreateChild<T>(this, std::forward<Args>(args)...));
}

template<typename T>
T* UI::Canvas::CreateChildBefore(UI::Element* youngerSibling) {
    return dynamic_cast<T*>(Element::CreateChildBefore<T>(this, youngerSibling));
}

template<typename T, typename ...Args>
T* UI::Canvas::CreateChildBefore(UI::Element* youngerSibling, Args&&... args) {
    return dynamic_cast<T*>(Element::CreateChildBefore<T>(this, youngerSibling, std::forward<Args>(args)...));
}

template<typename T>
T* UI::Canvas::CreateChildAfter(UI::Element* olderSibling) {
    return dynamic_cast<T*>(Element::CreateChildAfter<T>(this, olderSibling));
}
template<typename T, typename ...Args>
T* UI::Canvas::CreateChildAfter(UI::Element* olderSibling, Args&&... args) {
    return dynamic_cast<T*>(Element::CreateChildAfter<T>(this, olderSibling, std::forward<Args>(args)...));
}

} //End UI