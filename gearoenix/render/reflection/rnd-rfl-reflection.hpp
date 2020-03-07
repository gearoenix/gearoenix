#ifndef GEAROENIX_RENDER_REFLECTION_REFLECTION_HPP
#define GEAROENIX_RENDER_REFLECTION_REFLECTION_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-static.hpp"
#include "../../math/math-vector-3d.hpp"
#include "rnd-rfl-type.hpp"
#include <vector>

namespace gearoenix::physics::collider {
class Sphere;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::texture {
class TextureCube;
}

namespace gearoenix::render::reflection {
class Reflection : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, reflection_type)
    GX_GETSET_VAL_PRT(bool, enabled, true)
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_UCPTR_PRT(physics::collider::Sphere, collider)
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, diffuse)
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, specular)
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
