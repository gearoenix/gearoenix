#include "gx-vk-cmr-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../pipeline/gx-vk-pip-cache.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"
#include "../shader/gx-vk-shd-module.hpp"
#include "gx-vk-cmr-camera.hpp"

#include <array>

namespace {
std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> create_bloom_compute_pipeline(
    const std::shared_ptr<gearoenix::vulkan::pipeline::Cache>& cache,
    const VkPipelineLayout layout,
    const char* const spv_path,
    std::shared_ptr<gearoenix::vulkan::shader::Module>& shader_module_out)
{
    const std::unique_ptr<gearoenix::platform::stream::Stream> stream(
        gearoenix::platform::stream::Asset::construct(spv_path));
    GX_ASSERT_D(stream);
    const auto file_content = stream->get_file_content();
    shader_module_out = std::make_shared<gearoenix::vulkan::shader::Module>(
        std::span { file_content.data(), file_content.size() });

    VkPipelineShaderStageCreateInfo stage_info;
    GX_SET_ZERO(stage_info);
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage_info.module = shader_module_out->get_vulkan_data();
    stage_info.pName = "main";

    VkComputePipelineCreateInfo compute_info;
    GX_SET_ZERO(compute_info);
    compute_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_info.stage = stage_info;
    compute_info.layout = layout;
    return gearoenix::vulkan::pipeline::Pipeline::construct_compute(
        std::shared_ptr(cache), compute_info);
}
}

void gearoenix::vulkan::camera::Manager::initialise_bloom()
{
    const auto dev = device::Logical::get().get_vulkan_data();

    constexpr std::array bindings {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr },
        VkDescriptorSetLayoutBinding {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr },
    };

    VkDescriptorSetLayoutCreateInfo dsl_info;
    GX_SET_ZERO(dsl_info);
    dsl_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl_info.bindingCount = static_cast<std::uint32_t>(bindings.size());
    dsl_info.pBindings = bindings.data();
    GX_VK_CHK(vkCreateDescriptorSetLayout(dev, &dsl_info, nullptr, &bloom_descriptor_set_layout));

    VkSamplerCreateInfo sampler_info;
    GX_SET_ZERO(sampler_info);
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.maxLod = VK_LOD_CLAMP_NONE;
    GX_VK_CHK(vkCreateSampler(dev, &sampler_info, nullptr, &bloom_sampler));

    bloom_pipeline_cache = std::make_shared<pipeline::Cache>();

    VkPushConstantRange push_range;
    GX_SET_ZERO(push_range);
    push_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    push_range.offset = 0;
    push_range.size = sizeof(BloomPushConstants);

    VkPipelineLayoutCreateInfo pl_info;
    GX_SET_ZERO(pl_info);
    pl_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pl_info.setLayoutCount = 1;
    pl_info.pSetLayouts = &bloom_descriptor_set_layout;
    pl_info.pushConstantRangeCount = 1;
    pl_info.pPushConstantRanges = &push_range;
    GX_VK_CHK(vkCreatePipelineLayout(dev, &pl_info, nullptr, &bloom_pipeline_layout));

    bloom_multiply_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, bloom_pipeline_layout,
        "vulkan/shader/bloom-multiply.comp.spv", bloom_multiply_shader_module);
    bloom_prefilter_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, bloom_pipeline_layout,
        "vulkan/shader/bloom-prefilter.comp.spv", bloom_prefilter_shader_module);
    bloom_horizontal_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, bloom_pipeline_layout,
        "vulkan/shader/bloom-horizontal.comp.spv", bloom_horizontal_shader_module);
    bloom_vertical_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, bloom_pipeline_layout,
        "vulkan/shader/bloom-vertical.comp.spv", bloom_vertical_shader_module);
    bloom_upsampler_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, bloom_pipeline_layout,
        "vulkan/shader/bloom-upsampler.comp.spv", bloom_upsampler_shader_module);
}

void gearoenix::vulkan::camera::Manager::terminate_bloom()
{
    bloom_multiply_pipeline = nullptr;
    bloom_prefilter_pipeline = nullptr;
    bloom_horizontal_pipeline = nullptr;
    bloom_vertical_pipeline = nullptr;
    bloom_upsampler_pipeline = nullptr;

    bloom_multiply_shader_module = nullptr;
    bloom_prefilter_shader_module = nullptr;
    bloom_horizontal_shader_module = nullptr;
    bloom_vertical_shader_module = nullptr;
    bloom_upsampler_shader_module = nullptr;

    bloom_pipeline_cache = nullptr;

    const auto dev = device::Logical::get().get_vulkan_data();
    if (nullptr != bloom_pipeline_layout) {
        vkDestroyPipelineLayout(dev, bloom_pipeline_layout, nullptr);
        bloom_pipeline_layout = nullptr;
    }
    if (nullptr != bloom_sampler) {
        vkDestroySampler(dev, bloom_sampler, nullptr);
        bloom_sampler = nullptr;
    }
    if (nullptr != bloom_descriptor_set_layout) {
        vkDestroyDescriptorSetLayout(dev, bloom_descriptor_set_layout, nullptr);
        bloom_descriptor_set_layout = nullptr;
    }
}

void gearoenix::vulkan::camera::Manager::initialise_ctaa()
{
    VkPushConstantRange push_range;
    GX_SET_ZERO(push_range);
    push_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    push_range.offset = 0;
    push_range.size = sizeof(ColourCorrectionPushConstants);

    VkPipelineLayoutCreateInfo pl_info;
    GX_SET_ZERO(pl_info);
    pl_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pl_info.setLayoutCount = 1;
    pl_info.pSetLayouts = &bloom_descriptor_set_layout;
    pl_info.pushConstantRangeCount = 1;
    pl_info.pPushConstantRanges = &push_range;
    GX_VK_CHK(vkCreatePipelineLayout(device::Logical::get().get_vulkan_data(), &pl_info, nullptr, &ctaa_pipeline_layout));

    ctaa_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, ctaa_pipeline_layout,
        "vulkan/shader/ctaa.comp.spv", ctaa_shader_module);
}

void gearoenix::vulkan::camera::Manager::terminate_ctaa()
{
    ctaa_pipeline = nullptr;
    ctaa_shader_module = nullptr;

    const auto dev = device::Logical::get().get_vulkan_data();
    if (nullptr != ctaa_pipeline_layout) {
        vkDestroyPipelineLayout(dev, ctaa_pipeline_layout, nullptr);
        ctaa_pipeline_layout = nullptr;
    }
}

gearoenix::vulkan::camera::Manager::Manager()
    : Singleton<Manager>(this)
    , camera_uniform_indexer(Camera::max_count)
    , cameras_joint_models_uniform_indexer(render::record::Camera::cameras_joint_models_max_count)
{
    core::ecs::ComponentType::add<Camera>();
    initialise_bloom();
    initialise_ctaa();
}

gearoenix::vulkan::camera::Manager::~Manager()
{
    terminate_ctaa();
    terminate_bloom();
}

void gearoenix::vulkan::camera::Manager::build(std::string&& name, core::ecs::Entity* parent, core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    build_impl(std::move(name), parent, entity_callback);
    auto* const entity = entity_callback.get_return().get();
    auto transform = entity->get_component_shared_ptr<physics::Transformation>();
    Camera::construct(
        entity, entity->get_object_name() + "-vk-camera",
        core::job::EndCallerShared<Camera>([end = std::move(entity_callback)](std::shared_ptr<Camera>&& c) {
            end.get_return()->add_component(std::move(c));
        }),
        std::move(transform));
}

void gearoenix::vulkan::camera::Manager::window_resized()
{
    render::camera::Manager::window_resized();
}

void gearoenix::vulkan::camera::Manager::update()
{
    camera_uniform_indexer.reset();
    cameras_joint_models_uniform_indexer.reset();
    render::camera::Manager::update();
}

void gearoenix::vulkan::camera::Manager::upload_uniforms()
{
    camera_uniform_indexer.update();
    cameras_joint_models_uniform_indexer.update();
}
#endif
