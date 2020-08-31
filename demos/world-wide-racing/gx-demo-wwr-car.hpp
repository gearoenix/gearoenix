#ifndef GX_DEMO_WWR_CAR_HPP
#define GX_DEMO_WWR_CAR_HPP

#include <gearoenix/core/cr-static.hpp>
#include <memory>
#include <string>

namespace gearoenix::render::model {
class Dynamic;
}

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::demo::wwr {
class Car final {
    GX_GET_REFC_PRV(std::string, name)
    GX_GET_REFC_PRV(std::shared_ptr<render::scene::Scene>, scene)
    GX_GET_CREF_PRV(std::shared_ptr<render::model::Dynamic>, body_model)
    GX_GET_CREF_PRV(std::shared_ptr<render::model::Dynamic>, front_left_wheel_model)
    GX_GET_CREF_PRV(std::shared_ptr<render::model::Dynamic>, front_right_wheel_model)
    GX_GET_CREF_PRV(std::shared_ptr<render::model::Dynamic>, rear_left_wheel_model)
    GX_GET_CREF_PRV(std::shared_ptr<render::model::Dynamic>, rear_right_wheel_model)
private:
    void loaded() noexcept;

public:
    Car(std::string name, std::shared_ptr<render::scene::Scene> scene) noexcept;
    ~Car() noexcept;
};
}

#endif