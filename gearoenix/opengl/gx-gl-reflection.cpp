#include "gx-gl-reflection.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::ReflectionProbe::~ReflectionProbe() = default;

gearoenix::gl::BakedReflection::BakedReflection(
    std::string&& name,
    std::shared_ptr<TextureCube>&& irr,
    std::shared_ptr<TextureCube>&& rad,
    const math::Aabb3<double>& include_box)
    : Baked(core::ecs::ComponentType::create_index(this), std::move(irr), std::move(rad), include_box, std::move(name))
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

void gearoenix::gl::RuntimeReflection::initialise_gl()
{
    gl_environment = std::dynamic_pointer_cast<TextureCube>(environment);
    GX_ASSERT_D(nullptr != gl_environment);
    gl_environment_v = gl_environment->get_object();
    GX_ASSERT_D(0 != gl_environment_v);
    gl_irradiance = std::dynamic_pointer_cast<TextureCube>(irradiance);
    GX_ASSERT_D(nullptr != gl_irradiance);
    gl_irradiance_v = gl_irradiance->get_object();
    GX_ASSERT_D(0 != gl_irradiance_v);
    gl_radiance = std::dynamic_pointer_cast<TextureCube>(radiance);
    GX_ASSERT_D(nullptr != gl_radiance);
    gl_radiance_v = gl_radiance->get_object();
    GX_ASSERT_D(0 != gl_radiance_v);
    for (std::uint32_t face_index = 0; face_index < 6; ++face_index) {
        gl_environment_targets[face_index] = std::dynamic_pointer_cast<Target>(environment_targets[face_index]);
        GX_ASSERT_D(nullptr != gl_environment_targets[face_index]);
        gl_environment_targets_v[face_index] = gl_environment_targets[face_index]->get_framebuffer();
        GX_ASSERT_D(0 != gl_environment_targets_v[face_index]);
        gl_irradiance_targets[face_index] = std::dynamic_pointer_cast<Target>(irradiance_targets[face_index]);
        GX_ASSERT_D(nullptr != gl_irradiance_targets[face_index]);
        gl_irradiance_targets_v[face_index] = gl_irradiance_targets[face_index]->get_framebuffer();
        GX_ASSERT_D(0 != gl_irradiance_targets_v[face_index]);
        gl_radiance_targets[face_index].resize(roughnesses.size());
        gl_radiance_targets_v[face_index].resize(roughnesses.size());
        for (std::uint32_t mip_level = 0; mip_level < gl_radiance_targets[face_index].size(); ++mip_level) {
            gl_radiance_targets[face_index][mip_level] = std::dynamic_pointer_cast<Target>(radiance_targets[face_index][mip_level]);
            GX_ASSERT_D(nullptr != gl_radiance_targets[face_index][mip_level]);
            gl_radiance_targets_v[face_index][mip_level] = gl_radiance_targets[face_index][mip_level]->get_framebuffer();
            GX_ASSERT_D(0 != gl_radiance_targets_v[face_index][mip_level]);
        }
    }
}

gearoenix::gl::RuntimeReflection::RuntimeReflection(
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    std::string&& name)
    : Runtime(core::ecs::ComponentType::create_index(this), receive_box, exclude_box, include_box, std::move(name))
    , gl_environment_targets_v()
    , gl_irradiance_targets_v()
{
}

void gearoenix::gl::RuntimeReflection::construct(
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    std::string&& name,
    const std::uint32_t environment_resolution,
    const std::uint32_t irradiance_resolution,
    const std::uint32_t radiance_resolution,
    core::job::EndCallerShared<RuntimeReflection>&& end_callback)
{
    auto self = Object::construct<RuntimeReflection>(receive_box, exclude_box, include_box, std::move(name));
    auto* const ptr = self.get();
    end_callback.set_return(std::move(self));
    ptr->set_runtime_reflection_self(environment_resolution, irradiance_resolution, radiance_resolution,
        core::job::EndCaller([end = std::move(end_callback)]() -> void {
            end.get_return()->initialise_gl();
        }));
}

gearoenix::gl::RuntimeReflection::~RuntimeReflection() = default;

gearoenix::core::ecs::EntityPtr gearoenix::gl::ReflectionManager::build_baked(
    std::string&& name,
    core::ecs::Entity* const parent,
    std::shared_ptr<render::texture::TextureCube>&& irradiance,
    std::shared_ptr<render::texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<BakedReflection>(
        entity->get_object_name() + "-gl-reflection",
        std::dynamic_pointer_cast<TextureCube>(std::move(irradiance)),
        std::dynamic_pointer_cast<TextureCube>(std::move(radiance)),
        include_box));
    return entity;
}

void gearoenix::gl::ReflectionManager::build_runtime(
    std::string&& name,
    core::ecs::Entity* const parent,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::uint32_t environment_resolution,
    const std::uint32_t irradiance_resolution,
    const std::uint32_t radiance_resolution,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    const auto* const ptr = entity.get();
    entity_callback.set_return(std::move(entity));
    RuntimeReflection::construct(
        receive_box,
        exclude_box,
        include_box,
        ptr->get_object_name() + "-gl-runtime-reflection",
        environment_resolution,
        irradiance_resolution,
        radiance_resolution,
        core::job::EndCallerShared<RuntimeReflection>([e = std::move(entity_callback)](std::shared_ptr<RuntimeReflection>&& r) -> void {
            e.get_return()->add_component(std::move(r));
        }));
}

gearoenix::gl::ReflectionManager::ReflectionManager()
    : Singleton<ReflectionManager>(this)
{
    core::ecs::ComponentType::add<ReflectionProbe, RuntimeReflection>();
    core::ecs::ComponentType::add<BakedReflection>();
    core::ecs::ComponentType::add<RuntimeReflection>();

    Singleton<TextureManager>::get().create_cube_from_colour({}, core::job::EndCallerShared<render::texture::TextureCube>([this](auto&& t) {
        auto gt1 = std::dynamic_pointer_cast<TextureCube>(std::move(t));
        auto gt2 = gt1;
        black = core::Object::construct<BakedReflection>(
            "reflection-default-black", std::move(gt1), std::move(gt2),
            math::Aabb3(math::Vec3(std::numeric_limits<double>::max()), -math::Vec3(std::numeric_limits<double>::max())));
    }));
}

gearoenix::gl::ReflectionManager::~ReflectionManager() = default;

#endif
