#include "gx-vk-img-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-img-image.hpp"

gearoenix::vulkan::image::Manager::Manager(engine::Engine* const e) noexcept
    : frame_upload_images(e->get_frames_count())
    , e(e)
{
}

gearoenix::vulkan::image::Manager::~Manager() noexcept = default;

void gearoenix::vulkan::image::Manager::upload(
    std::shared_ptr<Image> img,
    std::shared_ptr<buffer::Buffer> buff,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    GX_GUARD_LOCK(upload_images)
    upload_images.emplace_back(std::move(img), std::move(buff), call);
}

void gearoenix::vulkan::image::Manager::update(command::Buffer& cmd) noexcept
{
    auto& images = frame_upload_images[e->get_frame_number()];
    images.clear();
    {
        GX_GUARD_LOCK(upload_images)
        std::swap(upload_images, images);
    }
    GXTODO
    // todo check for sync
    for (const auto& img_call : images) {
        std::get<0>(img_call)->transit_for_writing(cmd);
    }
    // todo sync check
    for (const auto& img_call : images) {
        std::get<0>(img_call)->copy_from_buffer(cmd, *std::get<1>(img_call));
    }
    // todo sync check
    for (const auto& img_call : images) {
        std::get<0>(img_call)->transit_for_reading(cmd);
    }
    // todo sync check
}

#endif
