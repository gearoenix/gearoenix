#include "gx-vk-rfl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../pipeline/gx-vk-pip-cache.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../shader/gx-vk-shd-module.hpp"
#include "../texture/gx-vk-txt-cube.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "gx-vk-rfl-baked.hpp"
#include "gx-vk-rfl-runtime.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"

void gearoenix::vulkan::reflection::Manager::initialise_convolution_compute()
{
    const auto dev = device::Logical::get().get_vulkan_data();

    constexpr std::array bindings {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr
        },
        VkDescriptorSetLayoutBinding {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr
        },
    };

    VkDescriptorSetLayoutCreateInfo dsl_info;
    GX_SET_ZERO(dsl_info);
    dsl_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl_info.bindingCount = static_cast<std::uint32_t>(bindings.size());
    dsl_info.pBindings = bindings.data();
    GX_VK_CHK(vkCreateDescriptorSetLayout(dev, &dsl_info, nullptr, &convolution_descriptor_set_layout));

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
    GX_VK_CHK(vkCreateSampler(dev, &sampler_info, nullptr, &convolution_sampler));

    convolution_pipeline_cache = std::make_shared<pipeline::Cache>();

    {
        VkPushConstantRange push_range;
        GX_SET_ZERO(push_range);
        push_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        push_range.offset = 0;
        push_range.size = sizeof(Runtime::IrradiancePushConstants);

        VkPipelineLayoutCreateInfo pl_info;
        GX_SET_ZERO(pl_info);
        pl_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pl_info.setLayoutCount = 1;
        pl_info.pSetLayouts = &convolution_descriptor_set_layout;
        pl_info.pushConstantRangeCount = 1;
        pl_info.pPushConstantRanges = &push_range;
        GX_VK_CHK(vkCreatePipelineLayout(dev, &pl_info, nullptr, &irradiance_pipeline_layout));

        const std::unique_ptr<platform::stream::Stream> stream(platform::stream::Asset::construct("vulkan/shader/irradiance.comp.spv"));
        GX_ASSERT_D(stream);
        const auto file_content = stream->get_file_content();
        irradiance_shader_module = std::make_shared<shader::Module>(std::span { file_content.data(), file_content.size() });

        VkPipelineShaderStageCreateInfo stage_info;
        GX_SET_ZERO(stage_info);
        stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stage_info.module = irradiance_shader_module->get_vulkan_data();
        stage_info.pName = "main";

        VkComputePipelineCreateInfo compute_info;
        GX_SET_ZERO(compute_info);
        compute_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        compute_info.stage = stage_info;
        compute_info.layout = irradiance_pipeline_layout;
        irradiance_pipeline = pipeline::Pipeline::construct_compute(std::shared_ptr(convolution_pipeline_cache), compute_info);
    }

    // Radiance pipeline (different push constant size from irradiance)
    {
        VkPushConstantRange push_range;
        GX_SET_ZERO(push_range);
        push_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        push_range.offset = 0;
        push_range.size = sizeof(Runtime::RadiancePushConstants);

        VkPipelineLayoutCreateInfo pl_info;
        GX_SET_ZERO(pl_info);
        pl_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pl_info.setLayoutCount = 1;
        pl_info.pSetLayouts = &convolution_descriptor_set_layout;
        pl_info.pushConstantRangeCount = 1;
        pl_info.pPushConstantRanges = &push_range;
        GX_VK_CHK(vkCreatePipelineLayout(dev, &pl_info, nullptr, &radiance_pipeline_layout));

        const std::unique_ptr<platform::stream::Stream> stream(platform::stream::Asset::construct("vulkan/shader/radiance.comp.spv"));
        GX_ASSERT_D(stream);
        const auto file_content = stream->get_file_content();
        radiance_shader_module = std::make_shared<shader::Module>(std::span { file_content.data(), file_content.size() });

        VkPipelineShaderStageCreateInfo stage_info;
        GX_SET_ZERO(stage_info);
        stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stage_info.module = radiance_shader_module->get_vulkan_data();
        stage_info.pName = "main";

        VkComputePipelineCreateInfo compute_info;
        GX_SET_ZERO(compute_info);
        compute_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        compute_info.stage = stage_info;
        compute_info.layout = radiance_pipeline_layout;
        radiance_pipeline = pipeline::Pipeline::construct_compute(std::shared_ptr(convolution_pipeline_cache), compute_info);
    }
}

void gearoenix::vulkan::reflection::Manager::update()
{
    render::reflection::Manager::update();
    uniform_indexer.reset();
}

void gearoenix::vulkan::reflection::Manager::initialise_black()
{
    if (descriptor::Bindless::singleton_is_invalid() || render::texture::Manager::singleton_is_invalid()) {
        core::job::send_job_to_pool([this] {
            core::job::send_job(Singleton<engine::Engine>::get().get_jobs_thread_id(), [this] {
                initialise_black();
            });
        });
        return;
    }

    render::texture::Manager::get().create_cube_from_colour({}, core::job::EndCallerShared<render::texture::TextureCube>([this](std::shared_ptr<render::texture::TextureCube>&& irr) {
        auto rad = irr;
        black = core::Object::construct<Baked>(
            nullptr, "reflection-default-black", std::move(irr), std::move(rad),
            math::Aabb3(math::Vec3(std::numeric_limits<double>::max()), -math::Vec3(std::numeric_limits<double>::max())));
    }));
}

gearoenix::vulkan::reflection::Manager::Manager()
    : Singleton<Manager>(this)
    , uniform_indexer(render::reflection::Baked::max_count + render::reflection::Runtime::max_count)
{
    core::ecs::ComponentType::add<Probe, Runtime>();
    core::ecs::ComponentType::add<Baked>();
    core::ecs::ComponentType::add<Runtime>();

    initialise_convolution_compute();

    initialise_black();
}

gearoenix::vulkan::reflection::Manager::~Manager()
{
    radiance_pipeline = nullptr;
    radiance_shader_module = nullptr;
    irradiance_pipeline = nullptr;
    irradiance_shader_module = nullptr;
    convolution_pipeline_cache = nullptr;

    const auto dev = device::Logical::get().get_vulkan_data();
    if (nullptr != radiance_pipeline_layout) {
        vkDestroyPipelineLayout(dev, radiance_pipeline_layout, nullptr);
        radiance_pipeline_layout = nullptr;
    }
    if (nullptr != irradiance_pipeline_layout) {
        vkDestroyPipelineLayout(dev, irradiance_pipeline_layout, nullptr);
        irradiance_pipeline_layout = nullptr;
    }
    if (nullptr != convolution_sampler) {
        vkDestroySampler(dev, convolution_sampler, nullptr);
        convolution_sampler = nullptr;
    }
    if (nullptr != convolution_descriptor_set_layout) {
        vkDestroyDescriptorSetLayout(dev, convolution_descriptor_set_layout, nullptr);
        convolution_descriptor_set_layout = nullptr;
    }
}

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::reflection::Manager::build_baked(
    std::string&& name,
    core::ecs::Entity* const parent,
    std::shared_ptr<render::texture::TextureCube>&& irradiance,
    std::shared_ptr<render::texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<Baked>(entity.get(), entity->get_object_name() + "-gapi-reflection", std::move(irradiance), std::move(radiance), include_box));
    return entity;
}

void gearoenix::vulkan::reflection::Manager::build_runtime(
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
    auto* const ptr = entity.get();
    entity_callback.set_return(std::move(entity));
    Runtime::construct(
        ptr, receive_box, exclude_box, include_box,
        ptr->get_object_name() + "-gapi-runtime-reflection",
        environment_resolution, irradiance_resolution, radiance_resolution,
        core::job::EndCallerShared<Runtime>([e = std::move(entity_callback)](std::shared_ptr<Runtime>&& r) -> void {
            e.get_return()->add_component(std::move(r));
        }));
}

void gearoenix::vulkan::reflection::Manager::upload_uniforms()
{
    uniform_indexer.update();
}

void gearoenix::vulkan::reflection::Manager::submit(const VkCommandBuffer cmd) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.5f, 1.0f, 0.5f, "render-reflection-probes");

    core::ecs::World::get().synchronised_system<Runtime>([cmd](const auto* const, const auto* const r) {
        r->vk_update(cmd);
    });
}

#endif
