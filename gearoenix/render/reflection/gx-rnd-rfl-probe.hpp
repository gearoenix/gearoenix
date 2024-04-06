#ifndef GEAROENIX_RENDER_REFLECTION_PROBE_HPP
#define GEAROENIX_RENDER_REFLECTION_PROBE_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-aabb.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Probe : core::ecs::Component {
    GX_GET_VAL_PRT(std::size_t, radiance_mips_count, 0);
    GX_GET_CREF_PRT(math::Aabb3<double>, include_box);
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, irradiance);
    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, radiance);
    GX_GET_VAL_PRT(core::ecs::entity_id_t, scene_id, 0);
    GX_GET_RRF_PRT(engine::Engine, e);

protected:
    Probe(
        engine::Engine& e,
        std::type_index final_component_type_index,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box,
        std::string&& name);

public:
    ~Probe() override;

    virtual void set_scene_id(core::ecs::entity_id_t id);
};
}
#endif