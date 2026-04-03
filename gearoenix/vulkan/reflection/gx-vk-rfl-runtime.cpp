#include "gx-vk-rfl-runtime.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../texture/gx-vk-txt-cube.hpp"
#include "../texture/gx-vk-txt-target.hpp"
#include "gx-vk-rfl-manager.hpp"
#include <numbers>

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
static_assert(sizeof(gearoenix::vulkan::reflection::Runtime::RadiancePushConstants) == 3 * 4 * 4 + 3 * 4);
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
    radiance_lod_coefficient = static_cast<float>(roughnesses.size() - 1);

    const auto& mgr = core::Singleton<Manager>::get();
    const auto dev = device::Logical::get().get_vulkan_data();
    const auto& dev_raii = device::Logical::get().get_device();

    {
        constexpr std::array pool_sizes {
            vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, 6 },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageImage, 6 },
        };
        vk::DescriptorPoolCreateInfo dp_info { { }, static_cast<std::uint32_t>(irradiance_descriptor_sets.size()), pool_sizes };
        irradiance_descriptor_pool = vk::raii::DescriptorPool(dev_raii, dp_info);

        const std::array layouts {
            mgr.get_convolution_descriptor_set_layout(),
            mgr.get_convolution_descriptor_set_layout(),
            mgr.get_convolution_descriptor_set_layout(),
            mgr.get_convolution_descriptor_set_layout(),
            mgr.get_convolution_descriptor_set_layout(),
            mgr.get_convolution_descriptor_set_layout(),
        };
        GX_ASSERT_D(layouts.size() == irradiance_descriptor_sets.size());

        vk::DescriptorSetAllocateInfo ds_info { *irradiance_descriptor_pool, layouts };
        auto allocated = dev.allocateDescriptorSets(ds_info);
        for (std::size_t i = 0; i < irradiance_descriptor_sets.size(); ++i) {
            irradiance_descriptor_sets[i] = allocated[i];
        }

        const vk::DescriptorImageInfo env_info {
            mgr.get_convolution_sampler(),
            gapi_environment->get_view()->get_vulkan_data(),
            vk::ImageLayout::eShaderReadOnlyOptimal,
        };

        std::array<vk::DescriptorImageInfo, 6> irr_infos { };
        std::array<vk::WriteDescriptorSet, 12> flat_writes { };

        for (std::uint32_t fi = 0; fi < irr_infos.size(); ++fi) {
            auto& irr_info = irr_infos[fi];
            irr_info.imageLayout = vk::ImageLayout::eGeneral;
            irr_info.imageView = gapi_irradiance->get_mips()[fi][0]->get_vulkan_data();

            auto& write0 = flat_writes[fi * 2];
            write0.dstSet = irradiance_descriptor_sets[fi];
            write0.dstBinding = 0;
            write0.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            write0.setImageInfo(env_info);

            auto& write1 = flat_writes[fi * 2 + 1];
            write1.dstSet = irradiance_descriptor_sets[fi];
            write1.dstBinding = 1;
            write1.descriptorType = vk::DescriptorType::eStorageImage;
            write1.setImageInfo(irr_info);
        }
        dev.updateDescriptorSets(flat_writes, { });
    }

    {
        const auto mip_count = static_cast<std::uint32_t>(roughnesses.size());
        const auto total_sets = 6u * mip_count;

        const std::array pool_sizes {
            vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, total_sets },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageImage, total_sets },
        };
        vk::DescriptorPoolCreateInfo dp_info { { }, total_sets, pool_sizes };
        radiance_descriptor_pool = vk::raii::DescriptorPool(dev_raii, dp_info);

        std::vector layouts(total_sets, mgr.get_convolution_descriptor_set_layout());

        vk::DescriptorSetAllocateInfo ds_info { *radiance_descriptor_pool, layouts };
        auto flat_sets = dev.allocateDescriptorSets(ds_info);

        for (std::uint32_t fi = 0; fi < 6; ++fi) {
            radiance_descriptor_sets[fi].resize(mip_count);
            for (std::uint32_t li = 0; li < mip_count; ++li) {
                radiance_descriptor_sets[fi][li] = flat_sets[fi * mip_count + li];
            }
        }

        const vk::DescriptorImageInfo env_info {
            mgr.get_convolution_sampler(),
            gapi_environment->get_view()->get_vulkan_data(),
            vk::ImageLayout::eShaderReadOnlyOptimal,
        };

        std::vector<vk::WriteDescriptorSet> all_writes;
        std::vector<vk::DescriptorImageInfo> rad_infos(total_sets);
        all_writes.reserve(total_sets * 2);

        for (std::uint32_t fi = 0; fi < 6; ++fi) {
            for (std::uint32_t li = 0; li < mip_count; ++li) {
                const auto idx = fi * mip_count + li;
                const auto ds = radiance_descriptor_sets[fi][li];

                auto& rad_info = rad_infos[idx];
                rad_info.imageLayout = vk::ImageLayout::eGeneral;
                rad_info.imageView = gapi_radiance->get_mips()[fi][li]->get_vulkan_data();

                vk::WriteDescriptorSet write0;
                write0.dstSet = ds;
                write0.dstBinding = 0;
                write0.descriptorType = vk::DescriptorType::eCombinedImageSampler;
                write0.setImageInfo(env_info);
                all_writes.push_back(write0);

                vk::WriteDescriptorSet write1;
                write1.dstSet = ds;
                write1.dstBinding = 1;
                write1.descriptorType = vk::DescriptorType::eStorageImage;
                write1.setImageInfo(rad_infos[idx]);
                all_writes.push_back(write1);
            }
        }
        dev.updateDescriptorSets(all_writes, { });
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
    const math::Aabb3<core::fp_t>& receive_box,
    const math::Aabb3<core::fp_t>& exclude_box,
    const math::Aabb3<core::fp_t>& include_box,
    std::string&& name)
    : render::reflection::Runtime(entity, core::ecs::ComponentType::create_index(this), receive_box, exclude_box, include_box, std::move(name))
{
}

void gearoenix::vulkan::reflection::Runtime::construct(
    core::ecs::Entity* const entity,
    const math::Aabb3<core::fp_t>& receive_box,
    const math::Aabb3<core::fp_t>& exclude_box,
    const math::Aabb3<core::fp_t>& include_box,
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

void gearoenix::vulkan::reflection::Runtime::convolute_irradiance(const vk::CommandBuffer cmd) const
{
    const auto& mgr = core::Singleton<Manager>::get();
    const auto fi = state_irradiance_face;
    const auto& env_cube = *gapi_environment;
    const auto& irr_cube = *gapi_irradiance;
    const auto irr_width = irr_cube.get_view()->get_image()->get_image_width();

    env_cube.get_view()->get_image()->transit(cmd, image::TransitionRequest::shader_read(vk::PipelineStageFlagBits2::eComputeShader));

    {
        image::TransitionRequest req;
        req.layout = vk::ImageLayout::eGeneral;
        req.access = vk::AccessFlagBits2::eShaderStorageWrite;
        req.stage = vk::PipelineStageFlagBits2::eComputeShader;
        req.base_layer = fi;
        req.layer_count = 1;
        req.base_mip = 0;
        req.mip_count = 1;
        irr_cube.get_view()->get_image()->transit(cmd, req);
    }

    cmd.bindPipeline(vk::PipelineBindPoint::eCompute, mgr.get_irradiance_pipeline()->get_vulkan_data());
    const auto ds = irradiance_descriptor_sets[fi];
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, mgr.get_irradiance_pipeline_layout(), 0, ds, { });

    IrradiancePushConstants pc { };
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
    cmd.pushConstants(mgr.get_irradiance_pipeline_layout(), vk::ShaderStageFlagBits::eCompute, 0u, sizeof(IrradiancePushConstants), &pc);

    const auto group_count_x = (irr_width + 15u) >> 4u;
    const auto group_count_y = (irr_width + 15u) >> 4u;
    cmd.dispatch(group_count_x, group_count_y, 1u);

    irr_cube.get_view()->get_image()->transit(cmd,
        image::TransitionRequest::shader_read().with_layers(fi, 1).with_mips(0, 1));
}

void gearoenix::vulkan::reflection::Runtime::convolute_radiance(const vk::CommandBuffer cmd) const
{
    const auto& mgr = core::Singleton<Manager>::get();
    const auto fi = state_radiance_face;
    const auto li = state_radiance_level;
    const auto& env_cube = *gapi_environment;
    const auto& rad_cube = *gapi_radiance;
    const auto rad_width = static_cast<std::uint32_t>(rad_cube.get_view()->get_image()->get_image_width() >> li);

    env_cube.get_view()->get_image()->transit(cmd, image::TransitionRequest::shader_read(vk::PipelineStageFlagBits2::eComputeShader));

    {
        image::TransitionRequest req;
        req.layout = vk::ImageLayout::eGeneral;
        req.access = vk::AccessFlagBits2::eShaderStorageWrite;
        req.stage = vk::PipelineStageFlagBits2::eComputeShader;
        req.base_layer = fi;
        req.layer_count = 1;
        req.base_mip = li;
        req.mip_count = 1;
        rad_cube.get_view()->get_image()->transit(cmd, req);
    }

    cmd.bindPipeline(vk::PipelineBindPoint::eCompute, mgr.get_radiance_pipeline()->get_vulkan_data());
    const auto ds = radiance_descriptor_sets[fi][li];
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, mgr.get_radiance_pipeline_layout(), 0, ds, { });

    const auto roughness = roughnesses[li];
    const auto roughness_p_2 = roughness * roughness;
    const auto roughness_p_4 = roughness_p_2 * roughness_p_2;
    const auto env_resolution = static_cast<core::fp_t>(env_cube.get_view()->get_image()->get_image_width());
    const auto sa_texel = std::numbers::pi / (1.5 * env_resolution * env_resolution);

    RadiancePushConstants pc { };
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
    pc.image_size = rad_width;
    pc.roughness = static_cast<float>(roughness);
    pc.roughness_p_4 = static_cast<float>(roughness_p_4);
    pc.sa_texel = static_cast<float>(sa_texel);
    cmd.pushConstants(mgr.get_radiance_pipeline_layout(), vk::ShaderStageFlagBits::eCompute, 0u, sizeof(RadiancePushConstants), &pc);

    const auto group_count_x = (rad_width + 15u) >> 4u;
    const auto group_count_y = (rad_width + 15u) >> 4u;
    cmd.dispatch(group_count_x, group_count_y, 1u);

    rad_cube.get_view()->get_image()->transit(cmd,
        image::TransitionRequest::shader_read().with_layers(fi, 1).with_mips(li, 1));
}

void gearoenix::vulkan::reflection::Runtime::vk_update(const vk::CommandBuffer cmd) const
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
}

#endif