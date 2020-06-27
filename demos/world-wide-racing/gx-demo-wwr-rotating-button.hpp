#ifndef GX_DEMO_WWR_ROTATING_BUTTON_HPP
#define GX_DEMO_WWR_ROTATING_BUTTON_HPP

#include <gearoenix/core/cr-static.hpp>
#include <memory>
#include <vector>

namespace gearoenix::physics::animation {
    struct Animation;
}

namespace gearoenix::render::widget {
    class Button;
}

namespace gearoenix::demo::wwr {
    class RotatingButton final {
    GX_GET_CREF_PRV(std::shared_ptr<render::widget::Button>, button)
    GX_GET_CREF_PRV(std::vector<std::shared_ptr<physics::animation::Animation>>, animations)

    private:
        std::shared_ptr<double> rotation_speed = std::make_shared<double>(0.2);
    public:
        explicit RotatingButton(std::shared_ptr<render::widget::Button> button) noexcept;
        ~RotatingButton() noexcept;
    };
}

#endif