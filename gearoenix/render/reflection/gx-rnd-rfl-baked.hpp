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
    constexpr static TypeIndex TYPE_INDEX = 20;
    constexpr static std::uint32_t MAX_COUNT = 8;

protected:
    Baked(
        engine::Engine& e,
        TypeIndex final_component_type_index,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        core::ecs::entity_id_t entity_id);

public:
    ~Baked() override;
};
}