#include "gx-rnd-lt-light.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../scene/gx-rnd-scn-scene.hpp"

void gearoenix::render::light::Light::remove_from_shadow_cascaders() const noexcept
{
    if (light_type != Type::Directional)
        return;
    if (nullptr != parent_scene)
        parent_scene->remove_shadow_cascader(id);
}

void gearoenix::render::light::Light::add_to_shadow_cascaders() const noexcept
{
    if (light_type != Type::Directional || !shadow_enabled)
        return;
    if (nullptr != parent_scene)
        parent_scene->add_shadow_cascader(id);
}

gearoenix::render::light::Light::Light(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const Type light_type) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Light, std::move(name))
    , light_type(light_type)
    , e(e)
{
    color.read(f);
    shadow_enabled = f->read_bool();
}

gearoenix::render::light::Light::Light(
    const core::Id my_id,
    std::string name,
    engine::Engine* const e,
    const Type light_type) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Light, std::move(name))
    , light_type(light_type)
    , e(e)
{
}

void gearoenix::render::light::Light::enable_shadowing() noexcept
{
    shadow_enabled = true;
    add_to_shadow_cascaders();
}

void gearoenix::render::light::Light::disable_shadowing() noexcept
{
    shadow_enabled = false;
    remove_from_shadow_cascaders();
}

void gearoenix::render::light::Light::set_scene(scene::Scene* const s) noexcept
{
    if (nullptr != parent_scene) {
        remove_from_shadow_cascaders();
    }
    parent_scene = s;
    add_to_shadow_cascaders();
}
