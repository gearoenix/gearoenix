#include "gx-vk-cmr-camera.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../model/gx-vk-mdl-model.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../scene/gx-vk-scn-scene.hpp"
#include "../skybox/gx-vk-sky-skybox.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../texture/gx-vk-txt-target.hpp"
#include "gx-vk-cmr-manager.hpp"

#include <cstring>
#include <ranges>

#ifdef far
#undef far
#endif

namespace {
const std::shared_ptr<gearoenix::vulkan::texture::Texture2D>& get_bloom_tex(
    const gearoenix::vulkan::camera::Target::Default& def,
    const std::uint32_t tex_index)
{
    return std::get<gearoenix::vulkan::texture::Target::GapiAttachment::VARIANT_2D_INDEX>(
        def.targets[tex_index][0]->get_gapi_attachments()[0].texture);
}

constexpr auto mip_aspect(const std::uint32_t base, const std::uint32_t mip_index)
{
    return std::max(1u, base >> mip_index);
}
}

struct GxShaderDataCameraJointModel;
void gearoenix::vulkan::camera::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    auto vk_t = std::dynamic_pointer_cast<texture::Target>(t);
    rendering_colour_format = vk_t->get_colour_format();
    gapi_target.target = Target::Customised { .target = std::move(vk_t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::vulkan::camera::Camera::update_target(core::job::EndCaller<>&& end)
{
    render::camera::Camera::update_target(core::job::EndCaller([this, self = object_self.lock(), end = std::move(end)] {
        (void)end;
        (void)self;
        gapi_target = Target(target);
        const auto* const txt_target = std::holds_alternative<Target::Customised>(gapi_target.target)
            ? gapi_target.get_customised().target.get()
            : gapi_target.get_default().main.get();
        rendering_colour_format = txt_target->get_colour_format();
        initialise_bloom_descriptors();
    }));
}

gearoenix::vulkan::camera::Camera::Camera(core::ecs::Entity* const entity, const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform)
    : render::camera::Camera(entity, core::ecs::ComponentType::create_index(this), name, std::move(target), std::move(transform))
    , gapi_target(this->target)
{
}

void gearoenix::vulkan::camera::Camera::construct(core::ecs::Entity* const entity, const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform)
{
    c.set_return(Object::construct<Camera>(entity, name, render::camera::Target(), std::move(transform)));
    c.get_return()->initialise();
    c.get_return()->update_target(core::job::EndCaller([c] { c.get_return()->enable_bloom(); }));
}

gearoenix::vulkan::camera::Camera::~Camera()
{
    destroy_bloom_descriptors();
}

void gearoenix::vulkan::camera::Camera::render_shadow(const render::record::Camera& cmr, const VkCommandBuffer cmd, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.3f, 0.5f, 0.9f, "render-shadow-camera for camera: {}", object_name);

    pc.camera_index = shader_data_index;

    vkCmdSetFrontFace(cmd, y_flipped ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE);

    // Shadow's camera must always have a customised target.
    GX_ASSERT_D(target.is_customised());
    const auto render_scope = gapi_target.get_customised().target->create_rendering_scope(cmd);

    record_viewport(cmr, cmd);

    const auto render_models = [&](const auto& models) {
        for (const auto& camera_model : models | std::views::values) {
            GX_ASSERT_D(camera_model.first_mvp_index != static_cast<std::uint32_t>(-1));
            pc.camera_model_index = cameras_joint_model_indices[camera_model.first_mvp_index];
            core::cast_ptr<model::Model>(camera_model.model->model)->render_shadow(camera_model, cmd, pc, current_bound_pipeline);
        }
    };

    render_models(cmr.opaque_models);
    render_models(cmr.translucent_models);
}

void gearoenix::vulkan::camera::Camera::render_forward(const render::record::Camera& cmr, const render::record::Skyboxes& skyboxes, const VkCommandBuffer cmd, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.8f, 0.4f, 0.6f, "render-forward-camera for camera: {}", object_name);

    pc.camera_index = shader_data_index;

    vkCmdSetFrontFace(cmd, y_flipped ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE);

    const auto render_scope = [&] {
        if (target.is_customised()) {
            return gapi_target.get_customised().target->create_rendering_scope(cmd);
        }
        return gapi_target.get_default().main->create_rendering_scope(cmd);
    }();

    record_viewport(cmr, cmd);

    const auto& fp = pipeline::Manager::get().get_pipelines(rendering_colour_format);

    const auto render_models = [&](const auto& models) {
        for (const auto& camera_model : models | std::views::values) {
            pc.camera_model_index = camera_model.first_mvp_index != static_cast<std::uint32_t>(-1) ? cameras_joint_model_indices[camera_model.first_mvp_index] : 0;
            core::cast_ptr<model::Model>(camera_model.model->model)->render_forward(camera_model, fp, cmd, pc, current_bound_pipeline);
        }
    };

    render_models(cmr.opaque_models);
    render_forward_skyboxes(skyboxes, fp, cmd, pc, current_bound_pipeline);
    render_models(cmr.translucent_models);
}

void gearoenix::vulkan::camera::Camera::render_forward_skyboxes(
    const render::record::Skyboxes& skyboxes, const pipeline::FormatPipelines& fp, const VkCommandBuffer cmd, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.8f, 0.8f, 0.6f, "render-skyboxes for camera: {}", object_name);
    for (const auto& skybox_data : skyboxes.skyboxes | std::views::values) {
        core::cast_ptr<skybox::Skybox>(skybox_data.skybox)->render_forward(cmd, fp, pc, current_bound_pipeline);
    }
}

void gearoenix::vulkan::camera::Camera::render_bloom(const scene::Scene& scene, const VkCommandBuffer cmd) const
{
    if (!bloom_data.has_value() || !target.is_default() || nullptr == bloom_descriptor_pool) {
        return;
    }

    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.7f, 0.4f, 0.8f, "render-bloom in scene: {}, camera: {}", scene.get_object_name(), object_name);

    const auto& b = *bloom_data;
    const auto& mgr = core::Singleton<Manager>::get();
    const auto& def = gapi_target.get_default();
    const auto& tex0 = get_bloom_tex(def, 0);
    const auto& tex1 = get_bloom_tex(def, 1);
    auto* const tex0_img = tex0->get_view()->get_image().get();
    auto* const tex1_img = tex1->get_view()->get_image().get();

    const auto base_w = tex0_img->get_image_width();
    const auto base_h = tex0_img->get_image_height();

    const auto dispatch = [&](const std::uint32_t w, const std::uint32_t h) {
        vkCmdDispatch(cmd, (w + 15u) >> 4u, (h + 15u) >> 4u, 1u);
    };

    const auto transit_read = [&](image::Image* const img) {
        img->transit(cmd, image::TransitionRequest::shader_read(VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT));
    };

    const auto transit_general = [&](image::Image* const img, const std::uint32_t mip) {
        image::TransitionRequest req;
        req.layout = VK_IMAGE_LAYOUT_GENERAL;
        req.access = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        req.stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        img->transit(cmd, req.with_mips(mip, 1));
    };

    BloomPushConstants pc { };

    // Step 1: Multiply (tex0 mip 0 -> tex1 mip 0)
    {
        GX_VK_PUSH_DEBUG_GROUP(cmd, 0.7f, 0.5f, 0.8f, "bloom-multiply");
        transit_read(tex0_img);
        transit_general(tex1_img, 0);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_multiply_pipeline()->get_vulkan_data());
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_pipeline_layout(), 0, 1, &bloom_ds_tex0_to_tex1[0], 0, nullptr);
        pc.src_mip = 0.0f;
        pc.value = exposure.get_enabled() ? exposure.get_value() : 1.0f;
        vkCmdPushConstants(cmd, mgr.get_bloom_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
        dispatch(mip_aspect(base_w, 0), mip_aspect(base_h, 0));
    }

    // Step 2: Prefilter (tex1 mip 0 -> tex0 mip 1)
    {
        GX_VK_PUSH_DEBUG_GROUP(cmd, 0.7f, 0.5f, 0.8f, "bloom-prefilter");
        transit_read(tex1_img);
        transit_general(tex0_img, 1);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_prefilter_pipeline()->get_vulkan_data());
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_pipeline_layout(), 0, 1, &bloom_ds_tex1_to_tex0[1], 0, nullptr);
        pc.texel_size_x = 1.0f / static_cast<float>(base_w);
        pc.texel_size_y = 1.0f / static_cast<float>(base_h);
        pc.src_mip = 0.0f;
        pc.value = 0.0f;
        std::memcpy(pc.scatter_clamp_max_threshold_threshold_knee, b.get_scatter_clamp_max_threshold_threshold_knee().data(), 4 * sizeof(float));
        vkCmdPushConstants(cmd, mgr.get_bloom_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
        dispatch(mip_aspect(base_w, 1), mip_aspect(base_h, 1));
    }

    // Step 3: Downsample (layers 1 through GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT-1)
    for (int blur_layer = 1; blur_layer < GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT; ++blur_layer) {
        const auto src_mip = static_cast<std::uint32_t>(blur_layer);
        const auto dst_mip = src_mip + 1;
        const auto sw = mip_aspect(base_w, src_mip);
        const auto sh = mip_aspect(base_h, src_mip);

        // H-Blur: tex0[src_mip] -> tex1[src_mip]
        {
            GX_VK_PUSH_DEBUG_GROUP(cmd, 0.6f, 0.5f, 0.9f, "bloom-horizontal-{}", blur_layer);
            transit_read(tex0_img);
            transit_general(tex1_img, src_mip);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_horizontal_pipeline()->get_vulkan_data());
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_pipeline_layout(), 0, 1, &bloom_ds_tex0_to_tex1[src_mip], 0, nullptr);
            pc.texel_size_x = 1.0f / static_cast<float>(sw);
            pc.texel_size_y = 1.0f / static_cast<float>(sh);
            pc.src_mip = static_cast<float>(src_mip);
            pc.value = 0.0f;
            vkCmdPushConstants(cmd, mgr.get_bloom_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
            dispatch(sw, sh);
        }

        // V-Blur: tex1[src_mip] -> tex0[dst_mip]
        {
            GX_VK_PUSH_DEBUG_GROUP(cmd, 0.5f, 0.6f, 0.9f, "bloom-vertical-{}", blur_layer);
            transit_read(tex1_img);
            transit_general(tex0_img, dst_mip);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_vertical_pipeline()->get_vulkan_data());
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_pipeline_layout(), 0, 1, &bloom_ds_tex1_to_tex0[dst_mip], 0, nullptr);
            pc.texel_size_x = 1.0f / static_cast<float>(sw);
            pc.texel_size_y = 1.0f / static_cast<float>(sh);
            pc.src_mip = static_cast<float>(src_mip);
            pc.value = 0.0f;
            vkCmdPushConstants(cmd, mgr.get_bloom_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
            dispatch(mip_aspect(base_w, dst_mip), mip_aspect(base_h, dst_mip));
        }
    }

    // Step 4: Upsample (copy_index from GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT down to 1)
    for (std::uint32_t i = 0; i < GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT; ++i) {
        const auto copy_index = static_cast<std::uint32_t>(GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT) - i;
        const auto mip_index = copy_index - 1;

        // Copy: tex0[copy_index] -> tex1[copy_index] (identity via multiplying)
        {
            GX_VK_PUSH_DEBUG_GROUP(cmd, 0.8f, 0.5f, 0.7f, "bloom-copy-{}", copy_index);
            transit_read(tex0_img);
            transit_general(tex1_img, copy_index);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_multiply_pipeline()->get_vulkan_data());
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_pipeline_layout(), 0, 1, &bloom_ds_tex0_to_tex1[copy_index], 0, nullptr);
            pc.texel_size_x = 0.0f;
            pc.texel_size_y = 0.0f;
            pc.src_mip = static_cast<float>(copy_index);
            pc.value = 1.0001f;
            std::memset(pc.scatter_clamp_max_threshold_threshold_knee, 0, sizeof(pc.scatter_clamp_max_threshold_threshold_knee));
            vkCmdPushConstants(cmd, mgr.get_bloom_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
            dispatch(mip_aspect(base_w, copy_index), mip_aspect(base_h, copy_index));
        }

        // Upsample: tex1[mip_index & mip_index+1] -> tex0[mip_index]
        {
            GX_VK_PUSH_DEBUG_GROUP(cmd, 0.9f, 0.5f, 0.6f, "bloom-upsampler-{}", mip_index);
            transit_read(tex1_img);
            transit_general(tex0_img, mip_index);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_upsampler_pipeline()->get_vulkan_data());
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_bloom_pipeline_layout(), 0, 1, &bloom_ds_tex1_to_tex0[mip_index], 0, nullptr);
            pc.texel_size_x = 0.0f;
            pc.texel_size_y = 0.0f;
            pc.src_mip = static_cast<float>(mip_index);
            pc.value = b.get_scatter_clamp_max_threshold_threshold_knee().x;
            vkCmdPushConstants(cmd, mgr.get_bloom_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
            dispatch(mip_aspect(base_w, mip_index), mip_aspect(base_h, mip_index));
        }
    }
}

void gearoenix::vulkan::camera::Camera::initialise_bloom_descriptors()
{
    destroy_bloom_descriptors();

    if (!target.is_default()) {
        return;
    }

    const auto& mgr = core::Singleton<Manager>::get();
    const auto dev = device::Logical::get().get_vulkan_data();
    const auto& def = gapi_target.get_default();
    const auto& tex0 = get_bloom_tex(def, 0);
    const auto& tex1 = get_bloom_tex(def, 1);

    constexpr auto mips = GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT;
    constexpr auto total_sets = static_cast<std::uint32_t>(mips * 2);

    constexpr std::array pool_sizes {
        VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = total_sets },
        VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = total_sets },
    };
    VkDescriptorPoolCreateInfo dp_info;
    GX_SET_ZERO(dp_info);
    dp_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dp_info.maxSets = total_sets;
    dp_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
    dp_info.pPoolSizes = pool_sizes.data();
    GX_VK_CHK(vkCreateDescriptorPool(dev, &dp_info, nullptr, &bloom_descriptor_pool));

    std::array<VkDescriptorSetLayout, total_sets> layouts;
    layouts.fill(mgr.get_bloom_descriptor_set_layout());

    std::array<VkDescriptorSet, total_sets> all_sets { };
    VkDescriptorSetAllocateInfo ds_info;
    GX_SET_ZERO(ds_info);
    ds_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    ds_info.descriptorPool = bloom_descriptor_pool;
    ds_info.descriptorSetCount = total_sets;
    ds_info.pSetLayouts = layouts.data();
    GX_VK_CHK(vkAllocateDescriptorSets(dev, &ds_info, all_sets.data()));

    for (std::uint32_t i = 0; i < mips; ++i) {
        bloom_ds_tex0_to_tex1[i] = all_sets[i];
        bloom_ds_tex1_to_tex0[i] = all_sets[mips + i];
    }

    const VkDescriptorImageInfo tex0_full {
        mgr.get_bloom_sampler(),
        tex0->get_view()->get_vulkan_data(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    const VkDescriptorImageInfo tex1_full {
        mgr.get_bloom_sampler(),
        tex1->get_view()->get_vulkan_data(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    std::array<VkDescriptorImageInfo, mips> tex1_mip_infos { };
    std::array<VkDescriptorImageInfo, mips> tex0_mip_infos { };
    for (std::uint32_t i = 0; i < mips; ++i) {
        tex1_mip_infos[i] = { .imageView = tex1->get_mips()[i]->get_vulkan_data(), .imageLayout = VK_IMAGE_LAYOUT_GENERAL };
        tex0_mip_infos[i] = { .imageView = tex0->get_mips()[i]->get_vulkan_data(), .imageLayout = VK_IMAGE_LAYOUT_GENERAL };
    }

    std::array<std::array<VkWriteDescriptorSet, 2>, total_sets> writes { };
    GX_SET_ZERO(writes);
    for (std::uint32_t i = 0; i < mips; ++i) {
        auto& w0 = writes[i];

        w0[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        w0[0].dstSet = bloom_ds_tex0_to_tex1[i];
        w0[0].dstBinding = 0;
        w0[0].descriptorCount = 1;
        w0[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        w0[0].pImageInfo = &tex0_full;

        w0[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        w0[1].dstSet = bloom_ds_tex0_to_tex1[i];
        w0[1].dstBinding = 1;
        w0[1].descriptorCount = 1;
        w0[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        w0[1].pImageInfo = &tex1_mip_infos[i];

        auto& w1 = writes[mips + i];

        w1[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        w1[0].dstSet = bloom_ds_tex1_to_tex0[i];
        w1[0].dstBinding = 0;
        w1[0].descriptorCount = 1;
        w1[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        w1[0].pImageInfo = &tex1_full;

        w1[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        w1[1].dstSet = bloom_ds_tex1_to_tex0[i];
        w1[1].dstBinding = 1;
        w1[1].descriptorCount = 1;
        w1[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        w1[1].pImageInfo = &tex0_mip_infos[i];
    }

    vkUpdateDescriptorSets(dev, static_cast<std::uint32_t>(writes.size() * 2), &writes[0][0], 0, nullptr);
}

void gearoenix::vulkan::camera::Camera::destroy_bloom_descriptors()
{
    if (nullptr != bloom_descriptor_pool) {
        const auto dev = device::Logical::get().get_vulkan_data();
        vkDestroyDescriptorPool(dev, bloom_descriptor_pool, nullptr);
        bloom_descriptor_pool = nullptr;
        bloom_ds_tex0_to_tex1 = { };
        bloom_ds_tex1_to_tex0 = { };
    }
}

void gearoenix::vulkan::camera::Camera::render_colour_correction_anti_aliasing(const scene::Scene& scene, const VkCommandBuffer cmd) const
{
    if (!target.is_default() || nullptr == bloom_descriptor_pool) {
        return;
    }

    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.6f, 0.8f, 0.7f, "render-ctaa in scene: {}, camera: {}", scene.get_object_name(), object_name);

    const auto& mgr = core::Singleton<Manager>::get();
    const auto& def = gapi_target.get_default();
    const auto& tex0 = get_bloom_tex(def, 0);
    const auto& tex1 = get_bloom_tex(def, 1);
    auto* const tex0_img = tex0->get_view()->get_image().get();
    auto* const tex1_img = tex1->get_view()->get_image().get();

    tex0_img->transit(cmd, image::TransitionRequest::shader_read(VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT));
    {
        image::TransitionRequest req;
        req.layout = VK_IMAGE_LAYOUT_GENERAL;
        req.access = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        req.stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        tex1_img->transit(cmd, req.with_mips(0, 1));
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_ctaa_pipeline()->get_vulkan_data());
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, mgr.get_ctaa_pipeline_layout(), 0, 1, &bloom_ds_tex0_to_tex1[0], 0, nullptr);

    ColourCorrectionPushConstants pc { };
    switch (colour_tuning.get_index()) {
    case render::camera::ColourTuning::gamma_correction_index:
        pc.mode = 0.0f;
        pc.param_x = colour_tuning.get_gamma_correction().gamma_exponent.x;
        pc.param_y = colour_tuning.get_gamma_correction().gamma_exponent.y;
        pc.param_z = colour_tuning.get_gamma_correction().gamma_exponent.z;
        break;
    case render::camera::ColourTuning::multiply_index:
        pc.mode = 1.0f;
        pc.param_x = colour_tuning.get_multiply().scale.x;
        pc.param_y = colour_tuning.get_multiply().scale.y;
        pc.param_z = colour_tuning.get_multiply().scale.z;
        break;
    default:
        pc.mode = 2.0f;
        break;
    }

    vkCmdPushConstants(cmd, mgr.get_ctaa_pipeline_layout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);

    const auto w = tex1_img->get_image_width();
    const auto h = tex1_img->get_image_height();
    vkCmdDispatch(cmd, (w + 15u) >> 4u, (h + 15u) >> 4u, 1u);
}

void gearoenix::vulkan::camera::Camera::after_record(const std::uint64_t frame_number, const render::record::Camera& rc)
{
    if (frame_number == last_update_frame_number) {
        return;
    }
    last_update_frame_number = frame_number;

    cameras_joint_model_indices.clear();

    {
        auto sd = descriptor::UniformIndexer<GxShaderDataCamera>::get().get_next();
        shader_data_index = sd.get_index();
        auto& [vp, position_far] = *sd.get_ptr();
        position_far = { math::Vec3<float>(rc.transform->get_local_position()), far };
        vp = view_projection;
    }

    for (const auto& mvp : rc.mvps) {
        auto sd = descriptor::UniformIndexer<GxShaderDataCameraJointModel>::get().get_next();
        sd.get_ptr()->mvp = mvp;
        cameras_joint_model_indices.push_back(sd.get_index());
    }

    for (const auto& camera_model : rc.opaque_models | std::views::values) {
        core::cast_ptr<model::Model>(camera_model.model->model)->after_record(frame_number, camera_model);
    }

    for (const auto& camera_model : rc.translucent_models | std::views::values) {
        core::cast_ptr<model::Model>(camera_model.model->model)->after_record(frame_number, camera_model);
    }
}

void gearoenix::vulkan::camera::Camera::record_viewport(const render::record::Camera& cmr, const VkCommandBuffer cmd)
{
    const VkViewport viewport {
        .x = cmr.viewport_clip.x,
        .y = cmr.viewport_clip.y,
        .width = cmr.viewport_clip.z,
        .height = cmr.viewport_clip.w,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    const VkRect2D scissor {
        .offset = {
            .x = static_cast<std::int32_t>(cmr.viewport_clip.x),
            .y = static_cast<std::int32_t>(cmr.viewport_clip.y),
        },
        .extent = {
            .width = static_cast<std::uint32_t>(cmr.viewport_clip.z),
            .height = static_cast<std::uint32_t>(cmr.viewport_clip.w),
        },
    };
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}
#endif