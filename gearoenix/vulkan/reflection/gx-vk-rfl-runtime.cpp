#include "gx-vk-rfl-runtime.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../texture/gx-vk-txt-target.hpp"
#include "../texture/gx-vk-txt-cube.hpp"

void gearoenix::vulkan::reflection::Runtime::initialise_gapi()
{
    gapi_environment = std::dynamic_pointer_cast<texture::TextureCube>(environment);
    GX_ASSERT_D(gapi_environment);
    environment_texture_index = gapi_environment->get_view_index();
    environment_sampler_index = gapi_environment->get_sampler_index();

    set_textures(*core::cast_ptr<texture::TextureCube>(irradiance.get()), *core::cast_ptr<texture::TextureCube>(radiance.get()));

    for (std::uint32_t face_index = 0; face_index < 6; ++face_index) {
        gapi_environment_targets[face_index] = std::dynamic_pointer_cast<texture::Target>(environment_targets[face_index]);
        GX_ASSERT_D(nullptr != gapi_environment_targets[face_index]);

        gapi_irradiance_targets[face_index] = std::dynamic_pointer_cast<texture::Target>(irradiance_targets[face_index]);
        GX_ASSERT_D(nullptr != gapi_irradiance_targets[face_index]);

        gapi_radiance_targets[face_index].resize(roughnesses.size());
        for (std::uint32_t mip_level = 0; mip_level < gapi_radiance_targets[face_index].size(); ++mip_level) {
            gapi_radiance_targets[face_index][mip_level] = std::dynamic_pointer_cast<texture::Target>(radiance_targets[face_index][mip_level]);
            GX_ASSERT_D(nullptr != gapi_radiance_targets[face_index][mip_level]);
        }
    }
}

gearoenix::vulkan::reflection::Runtime::Runtime(
    core::ecs::Entity* const entity,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    std::string&& name)
    : render::reflection::Runtime(entity, core::ecs::ComponentType::create_index(this), receive_box, exclude_box, include_box, std::move(name))
{
}

void gearoenix::vulkan::reflection::Runtime::construct(
    core::ecs::Entity* const entity,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    std::string&& name,
    const std::uint32_t environment_resolution,
    const std::uint32_t irradiance_resolution,
    const std::uint32_t radiance_resolution,
    core::job::EndCallerShared<Runtime>&& end_callback)
{
    auto self = Object::construct<Runtime>(entity, receive_box, exclude_box, include_box, std::move(name));
    auto* const ptr = self.get();
    end_callback.set_return(std::move(self));
    ptr->set_runtime_reflection_self(environment_resolution, irradiance_resolution, radiance_resolution, core::job::EndCaller([end = std::move(end_callback)]() -> void {
        end.get_return()->initialise_gapi();
    }));
}

gearoenix::vulkan::reflection::Runtime::~Runtime() = default;

#endif