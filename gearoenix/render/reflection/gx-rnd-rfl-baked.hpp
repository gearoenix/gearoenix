#pragma once
#include "gx-rnd-rfl-probe.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Baked : Probe {
    constexpr static auto object_type_index = gearoenix_render_reflection_baked_type_index;
    constexpr static auto max_count = 8;
    constexpr static std::array all_parent_object_type_indices { Probe::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Probe::object_type_index };

protected:
    Baked(
        core::ecs::Entity* entity,
        core::object_type_index_t final_component_type_index,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box, std::string&& name);

public:
    ~Baked() override;
};
}