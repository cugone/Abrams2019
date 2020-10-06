#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/UI/Element.hpp"

#include <filesystem>
#include <memory>

class Renderer;

namespace UI {

class Panel;

class Widget {
public:
    Widget(Renderer& renderer, const std::filesystem::path& path);
    ~Widget();
    std::string name{"DEFAULT WIDGET"};
    void BeginFrame();
    void Update(TimeUtils::FPSeconds deltaSeconds);
    void Render() const;
    void DebugRender() const;
    void EndFrame();

    Renderer& GetRenderer() const;
protected:
    [[nodiscard]] bool LoadFromXML(const std::filesystem::path& path);
    void LoadUI(const XMLElement& element);

    [[nodiscard]] std::shared_ptr<Element> CreateWigetTypeFromTypename(std::string nameString, const XMLElement& elem);
private:
    [[nodiscard]] bool HasPanelChild(const XMLElement& elem);

    std::vector<std::shared_ptr<Element>> _elements{};
    Renderer& _renderer;
    Panel* _panel{};

    friend class Panel;
};

} // namespace UI
