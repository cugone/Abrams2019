#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/UI/Element.hpp"

#include <memory>

namespace a2de {
    class AnimatedSprite;

    namespace UI {

        class Panel;

        class PictureBox : public Element {
        public:
            explicit PictureBox(Panel* parent = nullptr);
            explicit PictureBox(const XMLElement& elem, Panel* parent = nullptr);
            virtual ~PictureBox() = default;

            void SetImage(std::unique_ptr<AnimatedSprite> sprite) noexcept;
            [[nodiscard]] const AnimatedSprite* const GetImage() const noexcept;
            virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
            virtual void Render(Renderer& renderer) const override;
            virtual void DebugRender(Renderer& renderer) const override;

            [[nodiscard]] Vector4 CalcDesiredSize() const noexcept override;

        protected:
        private:
            [[nodiscard]] bool LoadFromXml(const XMLElement& elem) noexcept;

            std::unique_ptr<AnimatedSprite> _sprite{};
        };

    } // namespace UI
} // namespace a2de
