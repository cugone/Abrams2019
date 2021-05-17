#include "Engine/UI/Widget.hpp"

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/Label.hpp"
#include "Engine/UI/Panel.hpp"
#include "Engine/UI/PictureBox.hpp"
#include "Thirdparty/TinyXML2/tinyxml2.h"

#include <memory>
#include <typeinfo>

namespace UI {

Widget::Widget(Renderer& renderer, const std::filesystem::path& path)
: _renderer(renderer) {
    {
        auto err_msg{"Failed loading Widget:\n" + path.string() + "\n is ill-formed."};
        GUARANTEE_OR_DIE(LoadFromXML(path), err_msg.c_str());
    }
}

Widget::~Widget() {
    _elements.clear();
    _elements.shrink_to_fit();

    _panel = nullptr;
}

void Widget::BeginFrame() {
    /* DO NOTHING */
}

void Widget::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(_panel) {
        _panel->Update(deltaSeconds);
    }
}

void Widget::Render() const {
    _renderer.SetMaterial(_renderer.GetMaterial("__2D"));
    if(_panel) {
        _panel->Render(_renderer);
    }
}

void Widget::DebugRender() const {
    if(_panel) {
        _panel->DebugRender(_renderer);
    }
}

void Widget::EndFrame() {
    /* DO NOTHING */
}

bool Widget::LoadFromXML(const std::filesystem::path& path) {
    if(!FileUtils::IsSafeReadPath(path)) {
        return false;
    }
    if(tinyxml2::XMLDocument doc; tinyxml2::XML_SUCCESS == doc.LoadFile(path.string().c_str())) {
        if(auto xml_ui = doc.RootElement()) {
            if(!HasPanelChild(*xml_ui)) {
                return false;
            }
            LoadUI(*xml_ui);
            return true;
        }
    }
    return false;
}

void Widget::LoadUI(const XMLElement& element) {
    DataUtils::ValidateXmlElement(element, "ui", "", "name", "canvas");
    name = DataUtils::ParseXmlAttribute(element, "name", name);
    const auto load_children = [this](const XMLElement& elem) {
        const auto* c_name = elem.Name();
        const auto strName = std::string{c_name ? c_name : ""};
        _elements.emplace_back(CreateWigetTypeFromTypename(strName, elem));
    };
    const auto load_all_children = [this, &load_children](const XMLElement& elem) {
        load_children(elem);
        DataUtils::ForEachChildElement(elem, std::string{}, load_children);
    };
    DataUtils::ForEachChildElement(element, std::string{}, load_all_children);
}

Renderer& Widget::GetRenderer() const {
    return _renderer;
}

std::shared_ptr<Element> Widget::CreateWigetTypeFromTypename(std::string nameString, const XMLElement& elem) {
    const auto childname = StringUtils::ToLowerCase(nameString);
    if(childname == "canvas") {
        auto c = std::make_shared<Canvas>(this, _renderer, elem);
        _panel = c.get();
        return c;
    } else if(childname == "label") {
        if(const auto* parent = elem.Parent(); parent->ToElement()) {
            const auto parent_name = DataUtils::ParseXmlAttribute(*parent->ToElement(), "name", "");
            const auto found = std::find_if(std::begin(_elements), std::end(_elements), [&parent_name](std::shared_ptr<Element>& element) { return element->GetName() == parent_name; });
            if(found != std::end(_elements)) {
                if(auto* foundAsPanel = dynamic_cast<Panel*>(found->get())) {
                    auto lbl = std::make_shared<Label>(elem, foundAsPanel);
                    return lbl;
                }
            }
        }
        return std::make_shared<Label>(elem);
    } else if(childname == "picturebox") {
        if(const auto* parent = elem.Parent(); parent->ToElement()) {
            const auto parent_name = DataUtils::ParseXmlAttribute(*parent->ToElement(), "name", "");
            const auto found = std::find_if(std::begin(_elements), std::end(_elements), [&parent_name](std::shared_ptr<Element>& element) { return element->GetName() == parent_name; });
            if(found != std::end(_elements)) {
                if(auto* foundAsPanel = dynamic_cast<Panel*>(found->get())) {
                    auto pic = std::make_shared<PictureBox>(elem, foundAsPanel);
                    return pic;
                }
            }
        }
        return std::make_shared<PictureBox>(elem);
    } else {
        return nullptr;
    }
}

bool Widget::HasPanelChild(const XMLElement& elem) {
    if(const auto* first_child = elem.FirstChildElement()) {
        const auto* elem_name_cstr = first_child->Name();
        const auto elem_name = std::string{elem_name_cstr ? elem_name_cstr : ""};
        return elem_name == "canvas" || "grid";
    }
    return false;
}

} // namespace UI