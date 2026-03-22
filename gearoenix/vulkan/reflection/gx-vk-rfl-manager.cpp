#include "gx-vk-rfl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../pipeline/gx-vk-pip-cache.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../shader/gx-vk-shd-module.hpp"
#include "../texture/gx-vk-txt-cube.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "gx-vk-rfl-baked.hpp"
#include "gx-vk-rfl-runtime.hpp"

void gearoenix::vulkan::reflection::Manager::initialise_convolution_compute()
{
    constexpr std::array bindings {
        vk::DescriptorSetLayoutBinding { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eCompute },
        vk::DescriptorSetLayoutBinding { 1, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eCompute },
    };

    vk::DescriptorSetLayoutCreateInfo dsl_info { { }, bindings };
    convolution_descriptor_set_layout = vk::raii::DescriptorSetLayout(device::Logical::get().get_device(), dsl_info);

    vk::SamplerCreateInfo sampler_info { };
    sampler_info.magFilter = vk::Filter::eLinear;
    sampler_info.minFilter = vk::Filter::eLinear;
    sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
    sampler_info.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    sampler_info.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    sampler_info.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    sampler_info.maxLod = vk::LodClampNone;
    convolution_sampler = vk::raii::Sampler(device::Logical::get().get_device(), sampler_info);

    convolution_pipeline_cache = std::make_shared<pipeline::Cache>();

    {
        vk::PushConstantRange push_range { vk::ShaderStageFlagBits::eCompute, 0, sizeof(Runtime::IrradiancePushConstants) };

        vk::PipelineLayoutCreateInfo pl_info { { }, *convolution_descriptor_set_layout, push_range };
        irradiance_pipeline_layout = vk::raii::PipelineLayout(device::Logical::get().get_device(), pl_info);

        const std::unique_ptr<platform::stream::Stream> stream(platform::stream::Asset::construct("vulkan/shader/irradiance.comp.spv"));
        GX_ASSERT_D(stream);
        const auto file_content = stream->get_file_content();
        irradiance_shader_module = std::make_shared<shader::Module>(std::span { file_content.data(), file_content.size() });

        vk::PipelineShaderStageCreateInfo stage_info { { }, vk::ShaderStageFlagBits::eCompute, irradiance_shader_module->get_vulkan_data(), "main" };

        vk::ComputePipelineCreateInfo compute_info { { }, stage_info, *irradiance_pipeline_layout };
        irradiance_pipeline = pipeline::Pipeline::construct_compute(std::shared_ptr(convolution_pipeline_cache), compute_info);
    }

    // Radiance pipeline (different push constant size from irradiance)
    {
        vk::PushConstantRange push_range { vk::ShaderStageFlagBits::eCompute, 0, sizeof(Runtime::RadiancePushConstants) };

        vk::PipelineLayoutCreateInfo pl_info { { }, *convolution_descriptor_set_layout, push_range };
        radiance_pipeline_layout = vk::raii::PipelineLayout(device::Logical::get().get_device(), pl_info);

        const std::unique_ptr<platform::stream::Stream> stream(platform::stream::Asset::construct("vulkan/shader/radiance.comp.spv"));
        GX_ASSERT_D(stream);
        const auto file_content = stream->get_file_content();
        radiance_shader_module = std::make_shared<shader::Module>(std::span { file_content.data(), file_content.size() });

        vk::PipelineShaderStageCreateInfo stage_info { { }, vk::ShaderStageFlagBits::eCompute, radiance_shader_module->get_vulkan_data(), "main" };

        vk::ComputePipelineCreateInfo compute_info { { }, stage_info, *radiance_pipeline_layout };
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

    render::texture::Manager::get().create_cube_from_colour({ }, core::job::EndCallerShared<render::texture::TextureCube>([this](std::shared_ptr<render::texture::TextureCube>&& irr) {
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

void gearoenix::vulkan::reflection::Manager::submit(const vk::CommandBuffer cmd) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.5f, 1.0f, 0.5f, "render-reflection-probes");

    core::ecs::World::get().synchronised_system<Runtime>([cmd](const auto* const, const auto* const r) {
        r->vk_update(cmd);
    });
}

#endif
