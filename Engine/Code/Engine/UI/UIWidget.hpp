#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/UI/UIElement.hpp"

#include <filesystem>
#include <memory>

class Renderer;
class UIPanel;

class UIWidget {
public:
    UIWidget(Renderer& renderer, const std::filesystem::path& path);
    ~UIWidget();
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

    [[nodiscard]] std::shared_ptr<UIElement> CreateWigetTypeFromTypename(std::string nameString, const XMLElement& elem);

private:
    [[nodiscard]] bool HasPanelChild(const XMLElement& elem);

    std::vector<std::shared_ptr<UIElement>> _elements{};
    Renderer& _renderer;
    UIPanel* _panel{};

    friend class UIPanel;
};
