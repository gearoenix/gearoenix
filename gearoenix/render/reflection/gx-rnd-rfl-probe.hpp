#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../buffer/gx-rnd-buf-uniform.hpp"

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Probe : core::ecs::Component {
    constexpr static auto max_count = 1024;
    constexpr static auto object_type_index = gearoenix_render_reflection_probe_type_index;

    GX_GET_VAL_PRT(std::uint64_t, radiance_mips_count, 0);
    GX_GET_CREF_PRT(math::Aabb3<core::fp_t>, include_box);
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, irradiance);
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, radiance);
    GX_GET_CREF_PRT(buffer::Uniform, uniform);

    Probe(
        core::ecs::Entity* entity,
        core::object_type_index_t final_component_type_index,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<core::fp_t>& include_box,
        std::string&& name);
    Probe(
        core::ecs::Entity* entity,
        core::object_type_index_t final_component_type_index,
        const math::Aabb3<core::fp_t>& include_box,
        std::string&& name);

public:
    ~Probe() override;
    virtual void set_radiance(std::shared_ptr<texture::TextureCube>&&);
    void update_uniform();
};
}