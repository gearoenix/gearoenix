#include "gx-vk-cmr-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../pipeline/gx-vk-pip-cache.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../shader/gx-vk-shd-module.hpp"
#include "gx-vk-cmr-camera.hpp"

#include <array>

namespace {
std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> create_bloom_compute_pipeline(
    const std::shared_ptr<gearoenix::vulkan::pipeline::Cache>& cache,
    const vk::PipelineLayout layout,
    const char* const spv_path,
    std::shared_ptr<gearoenix::vulkan::shader::Module>& shader_module_out)
{
    const std::unique_ptr<gearoenix::platform::stream::Stream> stream(
        gearoenix::platform::stream::Asset::construct(spv_path));
    GX_ASSERT_D(stream);
    const auto file_content = stream->get_file_content();
    shader_module_out = std::make_shared<gearoenix::vulkan::shader::Module>(
        std::span { file_content.data(), file_content.size() });

    const vk::PipelineShaderStageCreateInfo stage_info { { }, vk::ShaderStageFlagBits::eCompute, shader_module_out->get_vulkan_data(), "main" };

    const vk::ComputePipelineCreateInfo compute_info { { }, stage_info, layout };
    return gearoenix::vulkan::pipeline::Pipeline::construct_compute(std::shared_ptr(cache), compute_info);
}
}

void gearoenix::vulkan::camera::Manager::initialise_bloom()
{
    constexpr std::array bindings {
        vk::DescriptorSetLayoutBinding { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eCompute },
        vk::DescriptorSetLayoutBinding { 1, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eCompute },
    };

    const vk::DescriptorSetLayoutCreateInfo dsl_info { { }, bindings };
    bloom_descriptor_set_layout = vk::raii::DescriptorSetLayout(device::Logical::get().get_device(), dsl_info);

    constexpr vk::SamplerCreateInfo sampler_info {
        { },
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge,
        0.0f,
        false,
        0.0f,
        false,
        vk::CompareOp::eNever,
        0.0f,
        vk::LodClampNone
    };
    bloom_sampler = vk::raii::Sampler(device::Logical::get().get_device(), sampler_info);

    bloom_pipeline_cache = std::make_shared<pipeline::Cache>();

    vk::PushConstantRange push_range { vk::ShaderStageFlagBits::eCompute, 0, sizeof(BloomPushConstants) };

    const vk::PipelineLayoutCreateInfo pl_info { { }, *bloom_descriptor_set_layout, push_range };
    bloom_pipeline_layout = vk::raii::PipelineLayout(device::Logical::get().get_device(), pl_info);

    bloom_multiply_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, *bloom_pipeline_layout,
        "vulkan/shader/bloom-multiply.comp.spv", bloom_multiply_shader_module);
    bloom_prefilter_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, *bloom_pipeline_layout,
        "vulkan/shader/bloom-prefilter.comp.spv", bloom_prefilter_shader_module);
    bloom_horizontal_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, *bloom_pipeline_layout,
        "vulkan/shader/bloom-horizontal.comp.spv", bloom_horizontal_shader_module);
    bloom_vertical_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, *bloom_pipeline_layout,
        "vulkan/shader/bloom-vertical.comp.spv", bloom_vertical_shader_module);
    bloom_upsampler_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, *bloom_pipeline_layout,
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

    bloom_pipeline_layout = vk::raii::PipelineLayout(nullptr);
    bloom_sampler = vk::raii::Sampler(nullptr);
    bloom_descriptor_set_layout = vk::raii::DescriptorSetLayout(nullptr);
}

void gearoenix::vulkan::camera::Manager::initialise_ctaa()
{
    vk::PushConstantRange push_range { vk::ShaderStageFlagBits::eCompute, 0, sizeof(ColourCorrectionPushConstants) };

    const vk::PipelineLayoutCreateInfo pl_info { { }, *bloom_descriptor_set_layout, push_range };
    ctaa_pipeline_layout = vk::raii::PipelineLayout(device::Logical::get().get_device(), pl_info);

    ctaa_pipeline = create_bloom_compute_pipeline(bloom_pipeline_cache, *ctaa_pipeline_layout,
        "vulkan/shader/ctaa.comp.spv", ctaa_shader_module);
}

void gearoenix::vulkan::camera::Manager::terminate_ctaa()
{
    ctaa_pipeline = nullptr;
    ctaa_shader_module = nullptr;

    ctaa_pipeline_layout = vk::raii::PipelineLayout(nullptr);
}

gearoenix::vulkan::camera::Manager::Manager()
    : Singleton<Manager>(this)
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
    render::camera::Manager::update();
}
#endif
