#include "gx-gl-reflection.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-entity-builder.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::ReflectionProbe::~ReflectionProbe() = default;

gearoenix::gl::BakedReflection::BakedReflection(
    std::string&& name,
    Engine& e,
    std::shared_ptr<TextureCube>&& irr,
    std::shared_ptr<TextureCube>&& rad,
    const math::Aabb3<double>& include_box,
    const core::ecs::entity_id_t entity_id)
    : Baked(e, core::ecs::ComponentType::create_index(this), std::move(irr), std::move(rad), include_box, std::move(name), entity_id)
{
    gl_irradiance = std::dynamic_pointer_cast<TextureCube>(irradiance);
    gl_radiance = std::dynamic_pointer_cast<TextureCube>(radiance);
    gl_irradiance_v = gl_irradiance->get_object();
    gl_radiance_v = gl_radiance->get_object();

    GX_ASSERT_D(nullptr != gl_irradiance);
    GX_ASSERT_D(0 != gl_irradiance_v);
    GX_ASSERT_D(nullptr != gl_radiance);
    GX_ASSERT_D(0 != gl_radiance_v);
}

gearoenix::gl::BakedReflection::~BakedReflection() = default;

gearoenix::gl::RuntimeReflection::RuntimeReflection(
    Engine& e,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Runtime(e, core::ecs::ComponentType::create_index(this), receive_box, exclude_box, include_box, std::move(name), entity_id)
    , gl_environment_targets_v()
    , gl_irradiance_targets_v()
{
}

void gearoenix::gl::RuntimeReflection::construct(
    Engine& e,
    const std::shared_ptr<render::reflection::Builder>& builder,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    std::string&& name,
    const std::uint32_t environment_resolution,
    const std::uint32_t irradiance_resolution,
    const std::uint32_t radiance_resolution,
    const core::ecs::entity_id_t entity_id,
    core::job::EndCallerShared<RuntimeReflection>&& end_callback)
{
    auto self = core::ecs::construct_component<RuntimeReflection>(e, receive_box, exclude_box, include_box, std::move(name), entity_id);
    end_callback.set_return(std::shared_ptr(self));
    self->set_runtime_reflection_self(
        self, builder, environment_resolution, irradiance_resolution, radiance_resolution,
        core::job::EndCaller([self, end = std::move(end_callback), builder]() -> void {
            (void)end;
            (void)builder;
            self->gl_environment = std::dynamic_pointer_cast<TextureCube>(self->environment);
            GX_ASSERT_D(nullptr != self->gl_environment);
            self->gl_environment_v = self->gl_environment->get_object();
            GX_ASSERT_D(0 != self->gl_environment_v);
            self->gl_irradiance = std::dynamic_pointer_cast<TextureCube>(self->irradiance);
            GX_ASSERT_D(nullptr != self->gl_irradiance);
            self->gl_irradiance_v = self->gl_irradiance->get_object();
            GX_ASSERT_D(0 != self->gl_irradiance_v);
            self->gl_radiance = std::dynamic_pointer_cast<TextureCube>(self->radiance);
            GX_ASSERT_D(nullptr != self->gl_radiance);
            self->gl_radiance_v = self->gl_radiance->get_object();
            GX_ASSERT_D(0 != self->gl_radiance_v);
            for (std::uint32_t face_index = 0; face_index < 6; ++face_index) {
                self->gl_environment_targets[face_index] = std::dynamic_pointer_cast<Target>(self->environment_targets[face_index]);
                GX_ASSERT_D(nullptr != self->gl_environment_targets[face_index]);
                self->gl_environment_targets_v[face_index] = self->gl_environment_targets[face_index]->get_framebuffer();
                GX_ASSERT_D(0 != self->gl_environment_targets_v[face_index]);
                self->gl_irradiance_targets[face_index] = std::dynamic_pointer_cast<Target>(self->irradiance_targets[face_index]);
                GX_ASSERT_D(nullptr != self->gl_irradiance_targets[face_index]);
                self->gl_irradiance_targets_v[face_index] = self->gl_irradiance_targets[face_index]->get_framebuffer();
                GX_ASSERT_D(0 != self->gl_irradiance_targets_v[face_index]);
                self->gl_radiance_targets[face_index].resize(self->roughnesses.size());
                self->gl_radiance_targets_v[face_index].resize(self->roughnesses.size());
                for (std::uint32_t mip_level = 0; mip_level < self->gl_radiance_targets[face_index].size(); ++mip_level) {
                    self->gl_radiance_targets[face_index][mip_level] = std::dynamic_pointer_cast<Target>(self->radiance_targets[face_index][mip_level]);
                    GX_ASSERT_D(nullptr != self->gl_radiance_targets[face_index][mip_level]);
                    self->gl_radiance_targets_v[face_index][mip_level] = self->gl_radiance_targets[face_index][mip_level]->get_framebuffer();
                    GX_ASSERT_D(0 != self->gl_radiance_targets_v[face_index][mip_level]);
                }
            }
        }));
}

gearoenix::gl::RuntimeReflection::~RuntimeReflection() = default;

gearoenix::gl::ReflectionBuilder::ReflectionBuilder(
    const std::string& name,
    physics::Transformation* const parent_transform,
    core::job::EndCaller<>&& end_callback)
    : Builder(std::string(name), parent_transform, std::move(end_callback))
{
}

gearoenix::gl::ReflectionBuilder::ReflectionBuilder(
    Engine& e,
    const std::string& name,
    physics::Transformation* const parent_transform,
    const math::Aabb3<double>& include_box,
    std::shared_ptr<render::texture::TextureCube>&& irradiance_texture,
    std::shared_ptr<render::texture::TextureCube>&& radiance_texture,
    core::job::EndCaller<>&& end_callback)
    : Builder(std::string(name), parent_transform, std::move(end_callback))
{
    auto& builder = entity_builder->get_builder();
    builder.add_component(core::ecs::construct_component<BakedReflection>(
        name + "-gl-reflection", e,
        std::dynamic_pointer_cast<TextureCube>(std::move(irradiance_texture)),
        std::dynamic_pointer_cast<TextureCube>(std::move(radiance_texture)),
        include_box,
        builder.get_id()));
}

void gearoenix::gl::ReflectionBuilder::construct_runtime(
    Engine& e,
    const std::string& name,
    physics::Transformation* const parent_transform,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::uint32_t environment_resolution,
    const std::uint32_t irradiance_resolution,
    const std::uint32_t radiance_resolution,
    core::job::EndCaller<>&& entity_end_callback,
    core::job::EndCallerShared<ReflectionBuilder>&& probe_end_callback)
{
    const std::shared_ptr<ReflectionBuilder> builder(new ReflectionBuilder(std::string(name), parent_transform, std::move(entity_end_callback)));
    RuntimeReflection::construct(
        e, builder, receive_box, exclude_box, include_box, std::string(name),
        environment_resolution, irradiance_resolution, radiance_resolution,
        builder->get_id(),
        core::job::EndCallerShared<RuntimeReflection>([e = std::move(probe_end_callback), b = builder](std::shared_ptr<RuntimeReflection>&& t) mutable -> void {
            b->entity_builder->get_builder().add_component(std::move(t));
            e.set_return(std::move(b));
        }));
}

gearoenix::gl::ReflectionBuilder::~ReflectionBuilder() = default;

std::shared_ptr<gearoenix::render::reflection::Builder> gearoenix::gl::ReflectionManager::build_baked(
    const std::string& name,
    physics::Transformation* const parent_transform,
    std::shared_ptr<render::texture::TextureCube>&& irradiance,
    std::shared_ptr<render::texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box,
    core::job::EndCaller<>&& end_callback)
{
    return std::make_shared<ReflectionBuilder>(
        eng,
        name,
        parent_transform,
        include_box,
        std::move(irradiance),
        std::move(radiance),
        std::move(end_callback));
}

void gearoenix::gl::ReflectionManager::build_runtime(
    const std::string& name,
    physics::Transformation* const parent_transform,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::uint32_t environment_resolution,
    const std::uint32_t irradiance_resolution,
    const std::uint32_t radiance_resolution,
    core::job::EndCaller<>&& entity_end_callback,
    core::job::EndCallerShared<render::reflection::Builder>&& probe_end_callback)
{
    ReflectionBuilder::construct_runtime(
        eng,
        name,
        parent_transform,
        receive_box,
        exclude_box,
        include_box,
        environment_resolution,
        irradiance_resolution,
        radiance_resolution,
        std::move(entity_end_callback),
        core::job::EndCallerShared<ReflectionBuilder>([e = std::move(probe_end_callback)](std::shared_ptr<ReflectionBuilder>&& b) -> void {
            e.set_return(std::move(b));
        }));
}

gearoenix::gl::ReflectionManager::ReflectionManager(Engine& e)
    : Manager(e)
    , eng(e)
{
    core::ecs::ComponentType::add<ReflectionProbe, RuntimeReflection>();
    core::ecs::ComponentType::add<BakedReflection>();
    core::ecs::ComponentType::add<RuntimeReflection>();
}

gearoenix::gl::ReflectionManager::~ReflectionManager() = default;

#endif
