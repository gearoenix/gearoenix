#ifndef GEAROENIX_PHYSICS_BODY_BODY_HPP
#define GEAROENIX_PHYSICS_BODY_BODY_HPP
#include "../../core/asset/gx-cr-asset.hpp"
#include "../../core/gx-cr-static.hpp"
#include "gx-phs-bd-type.hpp"
#include <memory>

namespace gearoenix::render::model {
struct Dynamic;
}

namespace gearoenix::physics::body {
struct Body : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, body_type)
    GX_GET_REFC_PRT(std::shared_ptr<render::model::Dynamic>, model)
    Body(core::Id id, std::string name, Type body_type, std::shared_ptr<render::model::Dynamic> model);

public:
    ~Body() override;
};
}
#endif
