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

namespace gearoenix::physics::accelerator {
class Bvh;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::texture {
class TextureCube;
}

namespace gearoenix::render::reflection {
/// Base class for reflection
///
/// The influencing length indicate the length of the AABB of collider that if any object place (exactly) in it,
/// the pbr materials of that object will get the properties of colliding reflection.
/// No reflection should overlaps with another one reflection.
/// \todo check for no reflection overlaps with another one reflection, and fire a error message
class Reflection : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, reflection_type)
    GX_GETSET_VAL_PRT(bool, enabled, true)
    GX_GET_VAL_PRT(bool, is_dynamic, false)
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_UCPTR_PRT(physics::collider::Aabb, collider)
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, irradiance)
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, radiance)
    GX_GET_VAL_PRT(double, influencing_length, std::numeric_limits<double>::max()) // because of runtime reflection
    /// It means it is hooked to model and do reflection for that object only
    GX_GETSET_VAL_PRT(bool, is_hooked, false)
    /// This is for keeping track of the dynamic models affected in previous frame and do clear reflection on them, ...
    GX_GET_CREF_PRT(std::vector<model::Model*>, affected_dynamic_models)
protected:
    Reflection(
        core::Id id,
        Type t,
        engine::Engine* e,
        const math::Vec3<double>& position = math::Vec3(0.0),
        double influence_radius = std::numeric_limits<double>::max()) noexcept;

public:
    ~Reflection() noexcept override;
    void update() noexcept;
    void check_dynamic_models(const physics::accelerator::Bvh* bvh) noexcept;
};
}
#endif
