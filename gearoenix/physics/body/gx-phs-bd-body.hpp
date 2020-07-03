#ifndef GEAROENIX_PHYSICS_BODY_BODY_HPP
#define GEAROENIX_PHYSICS_BODY_BODY_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-static.hpp"
#include "gx-phs-bd-type.hpp"
#include <memory>

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::physics::body {
class Body : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, body_type)
    GX_GET_REFC_PRT(std::shared_ptr<render::model::Model>, model)
    Body(core::Id id, std::string name, Type body_type, std::shared_ptr<render::model::Model> model) noexcept;

public:
    ~Body() noexcept override;
};
}
#endif
