#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::image {
struct Image;

struct Manager final : core::Singleton<Manager> {
    Manager();
    ~Manager() override;

    /// Uploads image data from multiple buffers <(Array or Face)<Mip>> to the image
    static void upload(std::shared_ptr<Image>&& img, std::vector<std::vector<std::shared_ptr<buffer::Buffer>>>&& buffs, bool generate_mipmaps, core::job::EndCaller<>&& end);
};
}
#endif
