#ifndef GEAROENIX_RENDER_REFLECTION_REFLECTION_HPP
#define GEAROENIX_RENDER_REFLECTION_REFLECTION_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-static.hpp"
#include "../../math/math-vector-3d.hpp"
#include "rnd-rfl-type.hpp"
#include <vector>

namespace gearoenix::physics::collider {
class Aabb;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::texture {
class TextureCube;
}

namespace gearoenix::render::reflection {
/// The influencing length indicate the length of the AABB of collider that if any object place (exactly) in it,
/// the pbr materials of that object will get the properties of colliding reflection.
class Reflection : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, reflection_type)
    GX_GETSET_VAL_PRT(bool, enabled, true)
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_UCPTR_PRT(physics::collider::Aabb, collider)
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, irradiance)
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, radiance)
    GX_GET_VAL_PRT(double, influencing_length, 5.0)
    GX_GETSET_VAL_PRT(bool, is_dynamic, false)
protected:
    Reflection(
        core::Id id,
        Type t,
        engine::Engine* e,
        const math::Vec3<double>& position = math::Vec3(0.0),
        double influence_radius = 10.0f) noexcept;

public:
    ~Reflection() noexcept override;
};
}
#endif
