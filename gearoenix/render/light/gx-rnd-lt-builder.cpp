#include "gx-rnd-lt-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../light/gx-rnd-lt-light.hpp"

gearoenix::render::light::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const DirectionalInfo&,
    const core::sync::EndCaller& end_callback) noexcept
    : e(e)
    , entity_builder(e.get_world()->create_shared_builder(core::sync::EndCaller(end_callback)))
{
    auto& b = entity_builder->get_builder();
    b.set_name(name);
    b.add_component(Light());
    b.add_component(Directional());
}

gearoenix::render::light::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const ShadowCasterDirectionalInfo& info,
    const core::sync::EndCaller& end_callback) noexcept
    : e(e)
    , entity_builder(e.get_world()->create_shared_builder(core::sync::EndCaller(end_callback)))
{
    auto& b = entity_builder->get_builder();
    b.set_name(name);
    b.add_component(Light());
    b.add_component(ShadowCasterDirectional(name, info.shadow_map_resolution, info.far, info.near, info.aspect, e, *this, end_callback));
}

gearoenix::render::light::Builder::~Builder() noexcept = default;

const gearoenix::render::light::ShadowCasterDirectional* gearoenix::render::light::Builder::get_shadow_caster_directional() const noexcept
{
    return entity_builder->get_builder().get_component<ShadowCasterDirectional>();
}

gearoenix::physics::Transformation& gearoenix::render::light::Builder::get_transformation(const std::size_t camera_index) noexcept
{
    return cameras[camera_index]->get_transformation();
}

gearoenix::render::light::Light& gearoenix::render::light::Builder::get_light() noexcept
{
    return *entity_builder->get_builder().get_component<Light>();
}
