#ifndef GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#define GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#include "../../math/gx-math-matrix-4d.hpp"
#include "gx-rnd-lt-light.hpp"

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::model {
struct Model;
}

namespace gearoenix::render::light {
struct Directional final : public Light {
    GX_GETSET_CREF_PRV(math::Vec3<double>, direction)
public:
    Directional(core::Id id, std::string name, platform::stream::Stream* f, engine::Engine* e) noexcept;
    Directional(core::Id id, std::string name, engine::Engine* e) noexcept;
    ~Directional() noexcept final;
    bool is_in_light(const model::Model*) const noexcept final;
};
}

#endif
