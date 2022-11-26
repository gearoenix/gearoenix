#include "gx-rnd-rfl-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-rfl-baked.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const math::Aabb3<double>& include_box,
    const std::shared_ptr<texture::TextureCube>& irradiance_texture,
    const std::shared_ptr<texture::TextureCube>& radiance_texture,
    const core::sync::EndCaller& end_callback) noexcept
    : entity_builder(e.get_world()->create_shared_builder(core::sync::EndCaller(end_callback)))
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(Baked(
        e,
        irradiance_texture,
        radiance_texture,
        include_box));
}

gearoenix::render::reflection::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::size_t environment_resolution,
    const std::size_t irradiance_resolution,
    const std::size_t radiance_resolution,
    const core::sync::EndCaller& end_callback) noexcept
    : entity_builder(e.get_world()->create_shared_builder(core::sync::EndCaller(end_callback)))
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(Runtime(
        e,
        *this,
        name,
        receive_box,
        exclude_box,
        include_box,
        environment_resolution,
        irradiance_resolution,
        radiance_resolution,
        end_callback));
    auto* const r = builder.get_component<Runtime>();
    builder.add_component(Baked(
        e,
        std::shared_ptr(r->get_irradiance()),
        std::shared_ptr(r->get_radiance()),
        include_box));
}

const gearoenix::render::reflection::Runtime& gearoenix::render::reflection::Builder::get_runtime() const noexcept
{
    const auto* const c = entity_builder->get_builder().get_component<Runtime>();
    GX_ASSERT(nullptr != c);
    return *c;
}

gearoenix::render::reflection::Runtime& gearoenix::render::reflection::Builder::get_runtime() noexcept
{
    auto* const c = entity_builder->get_builder().get_component<Runtime>();
    GX_ASSERT(nullptr != c);
    return *c;
}

gearoenix::render::reflection::Builder::~Builder() noexcept = default;

void gearoenix::render::reflection::Builder::set_camera_builder(std::shared_ptr<camera::Builder>&& builder, const std::size_t face_index) noexcept
{
    faces_camera_builders[face_index] = std::move(builder);
}
