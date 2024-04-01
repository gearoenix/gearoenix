#ifndef GEAROENIX_RENDER_REFLECTION_BAKED_HPP
#define GEAROENIX_RENDER_REFLECTION_BAKED_HPP
#include "gx-rnd-rfl-probe.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Baked : Probe {
    static constexpr std::size_t MAX_COUNT = 8;

protected:
    Baked(
        engine::Engine& e,
        std::type_index final_component_type_index,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box,
        std::string&& name);

public:
    ~Baked() override;
};
}
#endif