#include "gx-vk-rfl-runtime.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../texture/gx-vk-txt-cube.hpp"
#include "../texture/gx-vk-txt-target.hpp"
#include "gx-vk-rfl-manager.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"

namespace {
constexpr std::array face_uv_axis {
    std::array { gearoenix::math::Vec3(0.0f, 0.0f, -1.0f), gearoenix::math::Vec3(0.0f, -1.0f, 0.0f), gearoenix::math::Vec3(1.0f, 0.0f, 0.0f) }, // PositiveX
    std::array { gearoenix::math::Vec3(0.0f, 0.0f, 1.0f), gearoenix::math::Vec3(0.0f, -1.0f, 0.0f), gearoenix::math::Vec3(-1.0f, 0.0f, 0.0f) }, // NegativeX
    std::array { gearoenix::math::Vec3(1.0f, 0.0f, 0.0f), gearoenix::math::Vec3(0.0f, 0.0f, 1.0f), gearoenix::math::Vec3(0.0f, 1.0f, 0.0f) }, // PositiveY
    std::array { gearoenix::math::Vec3(1.0f, 0.0f, 0.0f), gearoenix::math::Vec3(0.0f, 0.0f, -1.0f), gearoenix::math::Vec3(0.0f, -1.0f, 0.0f) }, // NegativeY
    std::array { gearoenix::math::Vec3(1.0f, 0.0f, 0.0f), gearoenix::math::Vec3(0.0f, -1.0f, 0.0f), gearoenix::math::Vec3(0.0f, 0.0f, 1.0f) }, // PositiveZ
    std::array { gearoenix::math::Vec3(-1.0f, 0.0f, 0.0f), gearoenix::math::Vec3(0.0f, -1.0f, 0.0f), gearoenix::math::Vec3(0.0f, 0.0f, -1.0f) }, // NegativeZ
};

static_assert(sizeof(gearoenix::vulkan::reflection::Runtime::IrradiancePushConstants) == 3 * 4 * 4);
}

void gearoenix::vulkan::reflection::Runtime::initialise_gapi()
{
    gapi_environment = std::dynamic_pointer_cast<texture::TextureCube>(environment);
    GX_ASSERT_D(gapi_environment);

    gapi_irradiance = std::dynamic_pointer_cast<texture::TextureCube>(irradiance);
    GX_ASSERT_D(gapi_irradiance);

    gapi_radiance = std::dynamic_pointer_cast<texture::TextureCube>(radiance);
    GX_ASSERT_D(gapi_radiance);

    environment_texture_index = gapi_environment->get_view_index();
    environment_sampler_index = gapi_environment->get_sampler_index();

    set_textures(*gapi_irradiance, *gapi_radiance);

    {
        const auto& mgr = core::Singleton<Manager>::get();
        const auto dev = device::Logical::get().get_vulkan_data();

        constexpr std::array pool_sizes {
            VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 6 },
            VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = 6 },
        };
        VkDescriptorPoolCreateInfo dp_info;
        GX_SET_ZERO(dp_info);
        dp_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        dp_info.maxSets = irradiance_descriptor_sets.size();
        dp_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
        dp_info.pPoolSizes = pool_sizes.data();
        GX_VK_CHK(vkCreateDescriptorPool(dev, &dp_info, nullptr, &irradiance_descriptor_pool));

        const std::array layouts {
            mgr.get_irradiance_descriptor_set_layout(), mgr.get_irradiance_descriptor_set_layout(),
            mgr.get_irradiance_descriptor_set_layout(), mgr.get_irradiance_descriptor_set_layout(),
            mgr.get_irradiance_descriptor_set_layout(), mgr.get_irradiance_descriptor_set_layout(),
        };
        GX_ASSERT_D(layouts.size() == irradiance_descriptor_sets.size());

        VkDescriptorSetAllocateInfo ds_info;
        GX_SET_ZERO(ds_info);
        ds_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        ds_info.descriptorPool = irradiance_descriptor_pool;
        ds_info.descriptorSetCount = layouts.size();
        ds_info.pSetLayouts = layouts.data();
        GX_VK_CHK(vkAllocateDescriptorSets(dev, &ds_info, irradiance_descriptor_sets.data()));

        const VkDescriptorImageInfo env_info {
            mgr.get_irradiance_sampler(),
            gapi_environment->get_view()->get_vulkan_data(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        std::array<VkDescriptorImageInfo, 6> irr_infos{};
        std::array<std::array<VkWriteDescriptorSet, 2>, 6> writes{};

        for (std::uint32_t fi = 0; fi < irr_infos.size(); ++fi) {
            auto& irr_info = irr_infos[fi];
            irr_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            irr_info.imageView = gapi_irradiance->get_mips()[fi][0]->get_vulkan_data();

            auto& face_writes = writes[fi];

            auto& write0 = face_writes[0];
            write0.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write0.dstSet = irradiance_descriptor_sets[fi];
            write0.dstBinding = 0;
            write0.descriptorCount = 1;
            write0.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write0.pImageInfo = &env_info;

            auto& write1 = face_writes[1];
            write1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write1.dstSet = irradiance_descriptor_sets[fi];
            write1.dstBinding = 1;
            write1.descriptorCount = 1;
            write1.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            write1.pImageInfo = &irr_info;
        }
        vkUpdateDescriptorSets(dev, static_cast<std::uint32_t>(writes.size() * writes[0].size()), &writes[0][0], 0, nullptr);
    }

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

gearoenix::vulkan::reflection::Runtime::~Runtime()
{
    if (nullptr != irradiance_descriptor_pool) {
        vkDestroyDescriptorPool(device::Logical::get().get_vulkan_data(), irradiance_descriptor_pool, nullptr);
        irradiance_descriptor_pool = nullptr;
    }
}

void gearoenix::vulkan::reflection::Runtime::convolute_irradiance(const VkCommandBuffer cmd) const
{
    const auto& mgr = core::Singleton<Manager>::get();
    const auto fi = state_irradiance_face;
    const auto& env_cube = *gapi_environment;
    const auto& irr_cube = *gapi_irradiance;
    const auto irr_width = irr_cube.get_view()->get_image()->get_image_width();

    env_cube.get_view()->get_image()->transit(cmd, image::TransitionRequest::shader_read(VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT));

    {
        image::TransitionRequest req;
        req.layout = VK_IMAGE_LAYOUT_GENERAL;
        req.access = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        req.stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        req.base_layer = fi;
        req.layer_count = 1;
        req.base_mip = 0;
        req.mip_count = 1;
        irr_cube.get_view()->get_image()->transit(cmd, req);
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_irradiance_pipeline()->get_vulkan_data());
    const auto ds = irradiance_descriptor_sets[fi];
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_irradiance_pipeline_layout(), 0, 1, &ds, 0, nullptr);

    IrradiancePushConstants pc{};
    const auto& axes = face_uv_axis[fi];
    pc.u_axis[0] = axes[0].x;
    pc.u_axis[1] = axes[0].y;
    pc.u_axis[2] = axes[0].z;
    pc.u_axis[3] = 0.0f;
    pc.v_axis[0] = axes[1].x;
    pc.v_axis[1] = axes[1].y;
    pc.v_axis[2] = axes[1].z;
    pc.v_axis[3] = 0.0f;
    pc.face_axis[0] = axes[2].x;
    pc.face_axis[1] = axes[2].y;
    pc.face_axis[2] = axes[2].z;
    pc.image_size = irr_width;
    vkCmdPushConstants(cmd, mgr.get_irradiance_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(IrradiancePushConstants), &pc);

    const auto group_count_x = (irr_width + 15) / 16;
    const auto group_count_y = (irr_width + 15) / 16;
    vkCmdDispatch(cmd, group_count_x, group_count_y, 1);
}

void gearoenix::vulkan::reflection::Runtime::convolute_radiance(const VkCommandBuffer cmd) const
{

}

void gearoenix::vulkan::reflection::Runtime::vk_update(const VkCommandBuffer cmd) const
{
    switch (state) {
    case State::EnvironmentCubeMipMap:
        gapi_environment->generate_mipmap(cmd);
        return;
    case State::IrradianceFace:
        convolute_irradiance(cmd);
        return;
    case State::IrradianceMipMap:
        gapi_irradiance->generate_mipmap(cmd);
        return;
    case State::RadianceFaceLevel:
        convolute_radiance(cmd);
        return;
    default:
        return;
    }
    GX_UNEXPECTED;
}

#endif