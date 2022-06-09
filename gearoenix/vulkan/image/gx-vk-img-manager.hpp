#ifndef GEAROENIX_VULKAN_IMAGE_MANAGER_HPP
#define GEAROENIX_VULKAN_IMAGE_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include <tuple>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::image {
struct Image;
struct Manager final {
private:
    GX_CREATE_GUARD(upload_images)
    std::vector<std::tuple<
        std::shared_ptr<Image>,
        std::shared_ptr<buffer::Buffer>,
        core::sync::EndCaller<core::sync::EndCallerIgnore>>>
        upload_images;
    std::vector<decltype(upload_images)> frame_upload_images;
    engine::Engine* const e;

public:
    explicit Manager(engine::Engine* e) noexcept;
    ~Manager() noexcept;
    void upload(
        std::shared_ptr<Image> img,
        std::shared_ptr<buffer::Buffer> buff,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void update(command::Buffer& cmd) noexcept;
};
}

#endif
#endif