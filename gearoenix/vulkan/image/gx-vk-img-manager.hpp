#ifndef GEAROENIX_VULKAN_IMAGE_MANAGER_HPP
#define GEAROENIX_VULKAN_IMAGE_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <tuple>
#include <vector>

namespace gearoenix::vulkan::buffer {
class Buffer;
}

namespace gearoenix::vulkan::command {
class Buffer;
}

namespace gearoenix::vulkan::image {
class Image;
class Manager final {
private:
    GX_CREATE_GUARD(upload_images)
    std::vector<std::tuple<
        std::shared_ptr<Image>,
        std::shared_ptr<buffer::Buffer>,
        core::sync::EndCaller<core::sync::EndCallerIgnore>>>
        upload_images;

public:
    Manager() noexcept;
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