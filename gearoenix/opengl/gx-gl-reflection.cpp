#include "gx-gl-reflection.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::Reflection::Reflection() noexcept
    : core::ecs::Component(this)
{
}

gearoenix::gl::Reflection::~Reflection() noexcept = default;

gearoenix::gl::Reflection::Reflection(Reflection&&) noexcept = default;

gearoenix::gl::ReflectionRuntime::ReflectionRuntime() noexcept
    : core::ecs::Component(this)
    , environment_targets_v()
    , irradiance_targets_v()
    , radiance_targets_v()
{
}

gearoenix::gl::ReflectionRuntime::~ReflectionRuntime() noexcept = default;

gearoenix::gl::ReflectionRuntime::ReflectionRuntime(ReflectionRuntime&&) noexcept = default;

gearoenix::gl::ReflectionBuilder::ReflectionBuilder(
    Engine& e,
    const std::string& name,
    const math::Aabb3<double>& include_box,
    const std::shared_ptr<render::texture::TextureCube>& irradiance_texture,
    const std::shared_ptr<render::texture::TextureCube>& radiance_texture,
    const core::sync::EndCaller& end_callback) noexcept
    : render::reflection::Builder(e, name, include_box, irradiance_texture, radiance_texture, end_callback)
{
    auto& builder = entity_builder->get_builder();
    builder.add_component(Reflection());
    auto* const gb = builder.get_component<Reflection>();
    auto* const rb = builder.get_component<render::reflection::Baked>();
    gb->irradiance = std::dynamic_pointer_cast<TextureCube>(rb->get_irradiance());
    gb->radiance = std::dynamic_pointer_cast<TextureCube>(rb->get_radiance());
    e.todos.load([this, gb, rb, entity_builder = entity_builder, end_callback] {
        gb->irradiance_v = gb->irradiance->get_object();
        gb->radiance_v = gb->radiance->get_object();
    });
}

gearoenix::gl::ReflectionBuilder::ReflectionBuilder(
    Engine& e,
    const std::string& name,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::size_t environment_resolution,
    const std::size_t irradiance_resolution,
    const std::size_t radiance_resolution,
    const core::sync::EndCaller& end_callback) noexcept
    : render::reflection::Builder(
        e,
        name,
        receive_box,
        exclude_box,
        include_box,
        environment_resolution,
        irradiance_resolution,
        radiance_resolution,
        end_callback)
{
    auto& builder = entity_builder->get_builder();
    builder.add_components(ReflectionRuntime(), Reflection());
    auto* const gr = builder.get_component<ReflectionRuntime>();
    auto* const gb = builder.get_component<Reflection>();
    auto* const rr = builder.get_component<render::reflection::Runtime>();
    gr->environment = std::dynamic_pointer_cast<TextureCube>(rr->get_environment());
    gb->irradiance = std::dynamic_pointer_cast<TextureCube>(rr->get_irradiance());
    gb->radiance = std::dynamic_pointer_cast<TextureCube>(rr->get_radiance());
    for (std::size_t face_index = 0; face_index < 6; ++face_index) {
        gr->environment_targets[face_index] = std::dynamic_pointer_cast<Target>(rr->get_environment_targets()[face_index]);
        gr->irradiance_targets[face_index] = std::dynamic_pointer_cast<Target>(rr->get_irradiance_targets()[face_index]);
        auto& face_rad_targets = gr->radiance_targets[face_index];
        face_rad_targets.resize(rr->get_roughnesses().size());
        for (std::size_t mip_level = 0; mip_level < face_rad_targets.size(); ++mip_level) {
            face_rad_targets[mip_level] = std::dynamic_pointer_cast<Target>(rr->get_radiance_targets()[face_index][mip_level]);
        }
    }
    e.todos.load([this, gr, gb, rr, entity_builder = entity_builder, end_callback] {
        gr->environment_v = gr->environment->get_object();
        gb->irradiance_v = gb->irradiance->get_object();
        gb->radiance_v = gb->radiance->get_object();
        for (std::size_t face_index = 0; face_index < 6; ++face_index) {
            gr->environment_targets_v[face_index] = gr->environment_targets[face_index]->get_framebuffer();
            gr->irradiance_targets_v[face_index] = gr->irradiance_targets[face_index]->get_framebuffer();
            auto& face_rad_targets_v = gr->radiance_targets_v[face_index];
            face_rad_targets_v.resize(rr->get_roughnesses().size());
            for (std::size_t mip_level = 0; mip_level < face_rad_targets_v.size(); ++mip_level) {
                face_rad_targets_v[mip_level] = gr->radiance_targets[face_index][mip_level]->get_framebuffer();
            }
        }
    });
}

gearoenix::gl::ReflectionBuilder::~ReflectionBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::reflection::Builder> gearoenix::gl::ReflectionManager::build_baked(
    const std::string& name,
    const std::shared_ptr<render::texture::TextureCube>& irradiance,
    const std::shared_ptr<render::texture::TextureCube>& radiance,
    const math::Aabb3<double>& include_box,
    const core::sync::EndCaller& end_callback) noexcept
{
    return std::shared_ptr<ReflectionBuilder>(new ReflectionBuilder(
        eng,
        name,
        include_box,
        irradiance,
        radiance,
        end_callback));
}

std::shared_ptr<gearoenix::render::reflection::Builder> gearoenix::gl::ReflectionManager::build_runtime(
    const std::string& name,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::size_t environment_resolution,
    const std::size_t irradiance_resolution,
    const std::size_t radiance_resolution,
    const core::sync::EndCaller& end_callback) noexcept
{
    return std::shared_ptr<ReflectionBuilder>(new ReflectionBuilder(
        eng,
        name,
        receive_box,
        exclude_box,
        include_box,
        environment_resolution,
        irradiance_resolution,
        radiance_resolution,
        end_callback));
}

gearoenix::gl::ReflectionManager::ReflectionManager(Engine& e) noexcept
    : render::reflection::Manager(e)
    , eng(e)
{
    core::ecs::Component::register_type<Reflection>();
    core::ecs::Component::register_type<ReflectionRuntime>();
}

gearoenix::gl::ReflectionManager::~ReflectionManager() noexcept = default;

#endif
