#include "gx-gl-light.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/light/gx-rnd-lt-directional.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-loader.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::ShadowCasterDirectionalLight::ShadowCasterDirectionalLight() noexcept
    : core::ecs::Component(this)
{
}

gearoenix::gl::ShadowCasterDirectionalLight::~ShadowCasterDirectionalLight() noexcept = default;

gearoenix::gl::ShadowCasterDirectionalLight::ShadowCasterDirectionalLight(ShadowCasterDirectionalLight&&) noexcept = default;

gearoenix::gl::LightBuilder::LightBuilder(
    Engine& e,
    const std::string& name,
    const render::light::Builder::DirectionalInfo& info,
    const core::sync::EndCallerIgnored& end_callback) noexcept
    : render::light::Builder(e, name, info, end_callback)
    , eng(e)
{
}

gearoenix::gl::LightBuilder::LightBuilder(
    Engine& e,
    const std::string& name,
    const render::light::Builder::ShadowCasterDirectionalInfo& info,
    const core::sync::EndCallerIgnored& end_callback) noexcept
    : render::light::Builder(e, name, info, end_callback)
    , eng(e)
{
    auto& b = entity_builder->get_builder();
    const auto* const rl = get_shadow_caster_directional();
    ShadowCasterDirectionalLight l;
    l.shadow_map_target = std::dynamic_pointer_cast<Target>(rl->get_shadow_map_target());
    l.shadow_map_texture = std::dynamic_pointer_cast<Texture2D>(rl->get_shadow_map());
    b.add_component(std::move(l));
    e.todos.load([e = entity_builder]() noexcept {
        auto& l = *e->get_builder().get_component<ShadowCasterDirectionalLight>();
        l.shadow_map_target_v = l.shadow_map_target->get_framebuffer();
        l.shadow_map_texture_v = l.shadow_map_texture->get_object();
    });
}

gearoenix::gl::LightBuilder::~LightBuilder() noexcept = default;

#endif

std::shared_ptr<gearoenix::render::light::Builder> gearoenix::gl::LightManager::build_directional(
    const std::string& name, const core::sync::EndCallerIgnored& end_callback) noexcept
{
    return std::shared_ptr<render::light::Builder>(new LightBuilder(eng, name, render::light::Builder::DirectionalInfo {}, end_callback));
}

std::shared_ptr<gearoenix::render::light::Builder> gearoenix::gl::LightManager::build_shadow_caster_directional(
    const std::string& name,
    const std::size_t shadow_map_resolution,
    const float camera_far,
    const float camera_near,
    const float camera_aspect,
    const core::sync::EndCallerIgnored& end_callback) noexcept
{
    return std::shared_ptr<render::light::Builder>(new LightBuilder(
        eng,
        name,
        render::light::Builder::ShadowCasterDirectionalInfo {
            .shadow_map_resolution = shadow_map_resolution,
            .far = camera_far,
            .near = camera_near,
            .aspect = camera_aspect,
        },
        end_callback));
}

gearoenix::gl::LightManager::LightManager(Engine& e) noexcept
    : render::light::Manager(e)
    , eng(e)
{
}

gearoenix::gl::LightManager::~LightManager() noexcept = default;
