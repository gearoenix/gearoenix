#include "gx-vk-scn-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../camera/gx-vk-cmr-camera.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../engine/gx-vk-eng-frame.hpp"
#include "../gx-vk-marker.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../texture/gx-vk-txt-target.hpp"
#include "gx-vk-scn-scene.hpp"

#include <boost/container/flat_set.hpp>
#include <ranges>

namespace {
boost::container::flat_set<std::pair<double, gearoenix::vulkan::scene::Scene*>> scenes;
}

gearoenix::vulkan::scene::Manager::Manager()
    : Singleton<Manager>(this)
    , uniform_indexer(Scene::max_count)
{
    core::ecs::ComponentType::add<Scene>();
}

gearoenix::vulkan::scene::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::scene::Manager::build(std::string&& name, double layer) const
{
    auto entity = render::scene::Manager::build(std::move(name), layer);
    entity->add_component(core::Object::construct<Scene>(entity.get(), entity->get_object_name() + "-scene", layer));
    return entity;
}

void gearoenix::vulkan::scene::Manager::update()
{
    uniform_indexer.reset();
    scenes.clear();

    render::scene::Manager::update();

    core::ecs::World::get().synchronised_system<Scene>([&](const auto* const, Scene* const scene) -> void {
        if (!scene->get_enabled()) {
            return;
        }
        scenes.emplace(scene->get_layer(), scene);
        scene->after_record();
    });
}

void gearoenix::vulkan::scene::Manager::submit(const VkCommandBuffer vk_cmd)
{
    // TODO: render shadows
    // TODO: render reflection probes

    // if (render::engine::Engine::get().get_specification().is_deferred_supported) {
        // render_with_deferred();
        // GX_UNIMPLEMENTED;
    // } else {
        render_forward(vk_cmd);
    // }
}

void gearoenix::vulkan::scene::Manager::render_forward(const VkCommandBuffer vk_cmd)
{
    {
        GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 1.0f, 0.0f, 0.0f, "render-forward-all-scenes");
        VkPipeline current_bound_pipeline = nullptr;
        for (const auto& scene : scenes | std::views::values) {
            scene->render_forward(vk_cmd, current_bound_pipeline);
        }
    }

    // Combine all cameras to swapchain
    {
        auto& e = Singleton<engine::Engine>::get();

        GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 0.0f, 1.0f, 0.0f, "combine-all-cameras");

        const auto& frame = e.get_current_frame();
        const auto& swapchain_view = *frame.view;
        auto& swapchain_image = *swapchain_view.get_image();

        // Check if swapchain supports being a blit destination
        if ((swapchain_image.get_usage() & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0) {
            // TODO: Fall back to shader-based rendering to swapchain
            GX_LOG_D("Swapchain does not support blit destination, skipping camera composition.");
            return;
        }

        const auto vk_swapchain_image = swapchain_image.get_vulkan_data();
        const auto swapchain_width = swapchain_view.get_extent().width;
        const auto swapchain_height = swapchain_view.get_extent().height;

        bool first_camera = swapchain_image.get_layout() == VK_IMAGE_LAYOUT_UNDEFINED;

        for (auto* const scene : scenes | std::views::values) {
            for (const auto& record_cameras = scene->get_record().cameras; const auto camera_index : record_cameras.mains | std::views::values) {
                auto& cam = *core::cast_ptr<camera::Camera>(record_cameras.cameras[camera_index].camera);

                // Get the camera's render target colour attachment
                // Since bloom is not implemented yet, we blit from colours[0] (first colour attachment of the main target)
                // When bloom is implemented, this should use colours[1] or the final processed result
                const auto& cam_target = cam.get_gapi_target();
                if (cam_target.target.index() != camera::Target::default_var_index) {
                    continue; // Skip customised targets for now.
                }
                const auto& default_target = cam_target.get_default();
                if (!default_target.main) {
                    continue;
                }

                const auto& gapi_attachments = default_target.main->get_gapi_attachments();
                if (gapi_attachments.empty()) {
                    continue;
                }

                // Get the first colour attachment (colours[0])
                const auto& first_attachment = gapi_attachments[0];
                if (first_attachment.texture.index() != texture::Target::GapiAttachment::VARIANT_2D_INDEX) {
                    continue;
                }

                const auto& src_texture = std::get<texture::Target::GapiAttachment::VARIANT_2D_INDEX>(first_attachment.texture);
                const auto& src_view = *src_texture->get_view();
                auto& src_image = *src_view.get_image();
                const auto vk_src_image = src_image.get_vulkan_data();
                const auto src_width = static_cast<std::int32_t>(src_view.get_extent().width);
                const auto src_height = static_cast<std::int32_t>(src_view.get_extent().height);

                // Calculate destination rectangle with aspect ratio preservation
                const auto src_aspect = static_cast<float>(src_width) / static_cast<float>(src_height);
                const auto dst_aspect = static_cast<float>(swapchain_width) / static_cast<float>(swapchain_height);

                std::int32_t dst_x = 0, dst_y = 0;
                auto dst_width = static_cast<std::int32_t>(swapchain_width);
                auto dst_height = static_cast<std::int32_t>(swapchain_height);

                if (dst_aspect < src_aspect) {
                    // Pillarboxing (black bars on top/bottom)
                    dst_height = static_cast<std::int32_t>(static_cast<float>(swapchain_width) / src_aspect + 0.5f);
                    dst_y = (static_cast<std::int32_t>(swapchain_height) - dst_height) / 2;
                } else if (dst_aspect > src_aspect) {
                    // Letterboxing (black bars on left/right)
                    dst_width = static_cast<std::int32_t>(static_cast<float>(swapchain_height) * src_aspect + 0.5f);
                    dst_x = (static_cast<std::int32_t>(swapchain_width) - dst_width) / 2;
                }

                // Transition source image from COLOR_ATTACHMENT_OPTIMAL (after rendering) to TRANSFER_SRC_OPTIMAL
                // Note: After rendering completes, the image is in COLOR_ATTACHMENT_OPTIMAL layout.
                if (src_image.get_layout() != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
                    VkImageMemoryBarrier barrier;
                    GX_SET_ZERO(barrier);
                    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    barrier.oldLayout = src_image.get_layout();
                    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.image = vk_src_image;
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    barrier.subresourceRange.baseMipLevel = 0;
                    barrier.subresourceRange.levelCount = 1;
                    barrier.subresourceRange.baseArrayLayer = 0;
                    barrier.subresourceRange.layerCount = 1;

                    vkCmdPipelineBarrier(
                        vk_cmd,
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0, 0, nullptr, 0, nullptr, 1, &barrier);

                    src_image.set_layout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
                }

                // Transition swapchain image to TRANSFER_DST_OPTIMAL (with clear on first camera)
                if (swapchain_image.get_layout() != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                    VkImageMemoryBarrier barrier;
                    GX_SET_ZERO(barrier);
                    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    barrier.srcAccessMask = first_camera ? 0 : VK_ACCESS_TRANSFER_WRITE_BIT;
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    barrier.oldLayout = swapchain_image.get_layout();
                    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.image = vk_swapchain_image;
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    barrier.subresourceRange.baseMipLevel = 0;
                    barrier.subresourceRange.levelCount = 1;
                    barrier.subresourceRange.baseArrayLayer = 0;
                    barrier.subresourceRange.layerCount = 1;

                    vkCmdPipelineBarrier(
                        vk_cmd,
                        first_camera ? VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT : VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0, 0, nullptr, 0, nullptr, 1, &barrier);

                    swapchain_image.set_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
                }

                // Clear swapchain on first camera blit (to get black bars)
                if (first_camera) {
                    VkClearColorValue clear_color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
                    VkImageSubresourceRange clear_range;
                    GX_SET_ZERO(clear_range);
                    clear_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    clear_range.baseMipLevel = 0;
                    clear_range.levelCount = 1;
                    clear_range.baseArrayLayer = 0;
                    clear_range.layerCount = 1;
                    vkCmdClearColorImage(vk_cmd, vk_swapchain_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &clear_range);
                }

                // Perform blit
                VkImageBlit blit;
                GX_SET_ZERO(blit);
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { src_width, src_height, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = 0;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = { dst_x, dst_y, 0 };
                blit.dstOffsets[1] = { dst_x + dst_width, dst_y + dst_height, 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = 0;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                vkCmdBlitImage(
                    vk_cmd,
                    vk_src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    vk_swapchain_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit,
                    VK_FILTER_LINEAR);

                // Transition the source image back to COLOR_ATTACHMENT_OPTIMAL for the next frame's rendering.
                {
                    VkImageMemoryBarrier barrier;
                    GX_SET_ZERO(barrier);
                    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.image = vk_src_image;
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    barrier.subresourceRange.baseMipLevel = 0;
                    barrier.subresourceRange.levelCount = 1;
                    barrier.subresourceRange.baseArrayLayer = 0;
                    barrier.subresourceRange.layerCount = 1;

                    vkCmdPipelineBarrier(
                        vk_cmd,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        0, 0, nullptr, 0, nullptr, 1, &barrier);

                    src_image.set_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
                }

                first_camera = false;
            }
        }

        // If we blitted, transition swapchain to COLOR_ATTACHMENT_OPTIMAL for ImGui
        if (swapchain_image.get_layout() == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            VkImageMemoryBarrier barrier;
            GX_SET_ZERO(barrier);
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = vk_swapchain_image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(
                vk_cmd,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0, 0, nullptr, 0, nullptr, 1, &barrier);

            swapchain_image.set_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }
    }
}
#endif