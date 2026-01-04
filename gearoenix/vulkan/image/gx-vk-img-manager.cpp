#include "gx-vk-img-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-img-image.hpp"

gearoenix::vulkan::image::Manager::Manager(engine::Engine& e)
    : e(e)
{
}

gearoenix::vulkan::image::Manager::~Manager() = default;

// void gearoenix::vulkan::image::Manager::update(command::Buffer& cmd)
//{
//     auto& images = frame_upload_images[e->get_frame_number()];
//     images.clear();
//     {
//         GX_GUARD_LOCK(upload_images)
//         std::swap(upload_images, images);
//     }
//     GX_TODO
//     // todo check for sync
//     for (const auto& img_call : images) {
//         std::get<0>(img_call)->transit_for_writing(cmd);
//     }
//     // todo sync check
//     for (const auto& img_call : images) {
//         std::get<0>(img_call)->copy_from_buffer(cmd, *std::get<1>(img_call));
//     }
//     // todo sync check
//     for (const auto& img_call : images) {
//         std::get<0>(img_call)->transit_for_reading(cmd);
//     }
//     // todo sync check
// }

#endif
