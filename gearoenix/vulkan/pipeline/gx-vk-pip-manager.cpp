#include "gx-vk-pip-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../descriptor/gx-vk-des-set-layout.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../shader/gx-vk-shd-manager.hpp"
#include "../shader/gx-vk-shd-module.hpp"
#include "gx-vk-pip-cache.hpp"
#include "gx-vk-pip-layout.hpp"
#include "gx-vk-pip-pipeline.hpp"

static const char* const default_stage_entry = "main";

void gearoenix::vulkan::pipeline::Manager::initialize_ray_tracing() noexcept
{
    ray_gen_sm = shader_manager->get("camera.rgen");
    close_hit_sm = shader_manager->get("pbr.rchit");
    shadow_miss_sm = shader_manager->get("pbr-shadow.rmiss");
    miss_sm = shader_manager->get("pbr-sky.rmiss");

    stages_create_info.resize(4);
    GX_SET_VECTOR_ZERO(stages_create_info);
    shader_group_create_info.resize(4);
    GX_SET_VECTOR_ZERO(shader_group_create_info);

    // Ray gen part -----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t ray_gen_shader_index = 0;

        auto& ray_gen_stage_create_info = stages_create_info[ray_gen_shader_index];
        ray_gen_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ray_gen_stage_create_info.module = ray_gen_sm->get_vulkan_data();
        ray_gen_stage_create_info.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        ray_gen_stage_create_info.pName = default_stage_entry;

        auto& ray_gen_sg = shader_group_create_info[ray_gen_shader_index];
        ray_gen_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        ray_gen_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        ray_gen_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        ray_gen_sg.closestHitShader = VK_SHADER_UNUSED_KHR;
        ray_gen_sg.generalShader = ray_gen_shader_index;
        ray_gen_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }

    // Miss sky part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t miss_sky_shader_index = 1;

        auto& miss_sky_stage_create_info = stages_create_info[miss_sky_shader_index];
        miss_sky_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        miss_sky_stage_create_info.module = miss_sm->get_vulkan_data();
        miss_sky_stage_create_info.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
        miss_sky_stage_create_info.pName = default_stage_entry;

        auto& miss_sky_sg = shader_group_create_info[miss_sky_shader_index];
        miss_sky_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        miss_sky_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        miss_sky_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        miss_sky_sg.closestHitShader = VK_SHADER_UNUSED_KHR;
        miss_sky_sg.generalShader = miss_sky_shader_index;
        miss_sky_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }

    // Miss shadow part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t miss_shadow_shader_index = 2;

        auto& miss_shadow_stage_create_info = stages_create_info[miss_shadow_shader_index];
        miss_shadow_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        miss_shadow_stage_create_info.module = miss_sm->get_vulkan_data();
        miss_shadow_stage_create_info.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
        miss_shadow_stage_create_info.pName = default_stage_entry;

        auto& miss_shadow_sg = shader_group_create_info[miss_shadow_shader_index];
        miss_shadow_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        miss_shadow_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        miss_shadow_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        miss_shadow_sg.closestHitShader = VK_SHADER_UNUSED_KHR;
        miss_shadow_sg.generalShader = miss_shadow_shader_index;
        miss_shadow_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }

    // Close hit part ----------------------------------------------------------------------------------
    {
        constexpr std::uint32_t close_hit_shader_index = 3;

        auto& close_hit_stage_create_info = stages_create_info[close_hit_shader_index];
        close_hit_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        close_hit_stage_create_info.module = close_hit_sm->get_vulkan_data();
        close_hit_stage_create_info.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        close_hit_stage_create_info.pName = default_stage_entry;

        auto& close_hit_sg = shader_group_create_info[close_hit_shader_index];
        close_hit_sg.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        close_hit_sg.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        close_hit_sg.anyHitShader = VK_SHADER_UNUSED_KHR;
        close_hit_sg.closestHitShader = close_hit_shader_index;
        close_hit_sg.generalShader = VK_SHADER_UNUSED_KHR;
        close_hit_sg.intersectionShader = VK_SHADER_UNUSED_KHR;
    }
}

gearoenix::vulkan::pipeline::Manager::Manager(const engine::Engine& e) noexcept
    : cache(new Cache(e.get_logical_device()))
    , shader_manager(new shader::Manager(e))
{
    if (e.get_physical_device().get_rtx_supported())
        initialize_ray_tracing();
    else
        GX_UNIMPLEMENTED;
}

gearoenix::vulkan::pipeline::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Manager::create_ray_tracing_pbr(
    const std::shared_ptr<descriptor::SetLayout>& des_set_layout) noexcept
{
    return Pipeline::construct_ray_tracing(
        std::make_shared<Layout>(des_set_layout), cache, stages_create_info, shader_group_create_info);
}

#endif
