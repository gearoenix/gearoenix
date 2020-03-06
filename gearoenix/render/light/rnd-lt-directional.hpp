#ifndef GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#define GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#include "../../math/math-matrix-4d.hpp"
#include "rnd-lt-light.hpp"

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::light {
class Directional final : public Light {
    GX_GETSET_CREF_PRV(math::Vec3<float>, direction)
public:
    Directional(core::Id id, system::stream::Stream* f, engine::Engine* e) noexcept;
    Directional(core::Id id, engine::Engine* e) noexcept;
    ~Directional() noexcept final;
    bool is_in_light(const model::Model*) const noexcept final;
};
}

#endif
