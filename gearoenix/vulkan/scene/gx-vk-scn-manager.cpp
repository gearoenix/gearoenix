#include "gx-vk-scn-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../camera/gx-vk-cmr-camera.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-draw-state.hpp"
#include "../gx-vk-marker.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../reflection/gx-vk-rfl-manager.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "../texture/gx-vk-txt-target.hpp"
#include "gx-vk-scn-scene.hpp"

#include <boost/container/flat_set.hpp>

#include <ranges>

namespace {
boost::container::flat_set<std::pair<gearoenix::core::fp_t, gearoenix::vulkan::scene::Scene*>> scenes;
}

gearoenix::vulkan::scene::Manager::Manager()
    : Singleton<Manager>(this)
{
    core::ecs::ComponentType::add<Scene>();
}

gearoenix::vulkan::scene::Manager::~Manager() = default;

void gearoenix::vulkan::scene::Manager::build(std::string&& name, core::fp_t layer, core::job::EndCaller<core::ecs::EntityPtr>&& end) const
{
    auto entity = core::ecs::Entity::construct(std::move(name), nullptr);
    end.set_return(std::move(entity));
    Singleton<texture::Manager>::get().get_brdflut(core::job::EndCallerShared<render::texture::Texture2D>(
        [end = std::move(end), layer](std::shared_ptr<render::texture::Texture2D>&& t) {
            auto* const e = end.get_return().get();
            e->add_component(core::Object::construct<Scene>(e, e->get_object_name() + "-scene", layer, std::move(t)));
        }));
}

void gearoenix::vulkan::scene::Manager::update()
{
    render::scene::Manager::update();

    scenes.clear();
    core::ecs::World::get().synchronised_system<Scene>([&](const auto* const, Scene* const scene) -> void {
        if (!scene->get_enabled()) {
            return;
        }
        scenes.emplace(scene->get_layer(), scene);
    });
}

void gearoenix::vulkan::scene::Manager::submit(const vk::CommandBuffer cmd)
{
    DrawState draw_state;
    draw_state.command_buffer = cmd;
    pipeline::Manager::get().set(draw_state.pipelines);
    draw_state.gpu_buffer = Singleton<buffer::Manager>::get().get_device_root()->get_vulkan_data();
    draw_state.bindless_pipeline_layout = descriptor::Bindless::get().get_pipeline_layout();

    GX_PROFILE_EXP(render_shadows(draw_state));
    GX_PROFILE_EXP(Singleton<reflection::Manager>::get().submit(cmd));

    // if (render::engine::Engine::get().get_specification().is_deferred_supported) {
    // render_with_deferred();
    // GX_UNIMPLEMENTED;
    // } else {
    GX_PROFILE_EXP(render_forward(draw_state));
    // }
}

void gearoenix::vulkan::scene::Manager::render_forward(DrawState& draw_state)
{
    {
        GX_VK_PUSH_DEBUG_GROUP(draw_state.command_buffer, 1.0f, 0.0f, 0.0f, "render-forward-all-scenes");
        for (const auto& scene : scenes | std::views::values) {
            scene->render_forward(draw_state);
        }
    }

    // Combine all cameras to swapchain
    {
        GX_VK_PUSH_DEBUG_GROUP(draw_state.command_buffer, 0.0f, 1.0f, 0.0f, "combine-all-cameras");

        const auto& swapchain_view = *Singleton<Swapchain>::get().get_frame().view;
        auto& swapchain_image = *swapchain_view.get_image();

        // Check if the swapchain supports being a blit destination
        if (!(swapchain_image.get_usage() & vk::ImageUsageFlagBits::eTransferDst)) {
            GX_LOG_E("Swapchain does not support blit destination, skipping camera composition.");
            GX_UNIMPLEMENTED;
            return;
        }

        const auto vk_swapchain_image = swapchain_image.get_vulkan_data();
        const auto swapchain_width = swapchain_view.get_extent().width;
        const auto swapchain_height = swapchain_view.get_extent().height;

        bool first_camera = swapchain_image.get_layout() == vk::ImageLayout::eUndefined;

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

                const auto& gapi_attachments = default_target.targets[1][0]->get_gapi_attachments();
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
                src_image.transit(draw_state.command_buffer, image::TransitionRequest::transfer_src());

                // Transition swapchain image to TRANSFER_DST_OPTIMAL (with clear on first camera)
                swapchain_image.transit(draw_state.command_buffer, image::TransitionRequest::transfer_dst());

                // Clear swapchain on first camera blit (to get black bars)
                if (first_camera) {
                    constexpr vk::ClearColorValue clear_color { std::array { 0.0f, 0.0f, 0.0f, 1.0f } };
                    constexpr vk::ImageSubresourceRange clear_range { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
                    draw_state.command_buffer.clearColorImage(vk_swapchain_image, vk::ImageLayout::eTransferDstOptimal, clear_color, clear_range);
                }

                // Perform blit
                vk::ImageBlit blit;
                blit.srcOffsets[0] = vk::Offset3D { 0, 0, 0 };
                blit.srcOffsets[1] = vk::Offset3D { src_width, src_height, 1 };
                blit.srcSubresource = vk::ImageSubresourceLayers { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };
                blit.dstOffsets[0] = vk::Offset3D { dst_x, dst_y, 0 };
                blit.dstOffsets[1] = vk::Offset3D { dst_x + dst_width, dst_y + dst_height, 1 };
                blit.dstSubresource = vk::ImageSubresourceLayers { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };

                draw_state.command_buffer.blitImage(
                    vk_src_image, vk::ImageLayout::eTransferSrcOptimal, vk_swapchain_image,
                    vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eLinear);

                // Transition the source image back to COLOR_ATTACHMENT_OPTIMAL for the next frame's rendering.
                src_image.transit(draw_state.command_buffer, image::TransitionRequest::color_attachment());

                first_camera = false;
            }
        }

        // If we blitted, transition swapchain to COLOR_ATTACHMENT_OPTIMAL for ImGui
        if (swapchain_image.get_layout() == vk::ImageLayout::eTransferDstOptimal) {
            swapchain_image.transit(draw_state.command_buffer, image::TransitionRequest::color_attachment());
        }
    }
}

void gearoenix::vulkan::scene::Manager::render_shadows(DrawState& draw_state)
{
    GX_VK_PUSH_DEBUG_GROUP(draw_state.command_buffer, 1.0f, 0.0f, 1.0f, "render-shadow-all-scenes");
    for (const auto& scene : scenes | std::views::values) {
        scene->render_shadows(draw_state);
    }
}
#endif