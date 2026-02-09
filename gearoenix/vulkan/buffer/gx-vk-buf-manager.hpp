#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-build-configuration.hpp"
#include "../gx-vk-loader.hpp"

#include <array>
#include <memory>

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::image {
struct Image;
}

namespace gearoenix::vulkan::queue {
struct Queue;
}

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform;
struct Manager final : core::Singleton<Manager> {
    GX_GET_REFC_PRV(std::shared_ptr<Buffer>, upload_root_buffer);
    GX_GET_REFC_PRV(std::shared_ptr<Buffer>, gpu_root_buffer);

    const std::array<std::shared_ptr<Buffer>, frames_in_flight> each_frame_upload_source;
    const std::shared_ptr<Buffer> each_frame_upload_destination;


    [[nodiscard]] std::shared_ptr<Buffer> create_upload_root_buffer() const;
    [[nodiscard]] std::shared_ptr<Buffer> create_gpu_root_buffer() const;
    [[nodiscard]] std::array<std::shared_ptr<Buffer>, frames_in_flight> create_each_frame_upload_source() const;
    [[nodiscard]] std::shared_ptr<Buffer> create_each_frame_upload_destination() const;

public:
    Manager();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    ~Manager() override;

    [[nodiscard]] std::shared_ptr<Buffer> create_static(std::int64_t size);
    [[nodiscard]] std::shared_ptr<Buffer> create_staging(std::int64_t size);
    [[nodiscard]] std::shared_ptr<Uniform> create_uniform(std::int64_t size);
    [[nodiscard]] std::shared_ptr<Buffer> create(const std::string& name, const void* data, std::int64_t size, core::job::EndCaller<>&& end);

    template <typename T>
    [[nodiscard]] std::shared_ptr<Buffer> create(const std::string& name, const std::vector<T>& data, core::job::EndCaller<>&& end)
    {
        return create(name, data.data(), data.size() * sizeof(T), std::move(end));
    }

    void upload_dynamics(VkCommandBuffer vk_cmd);
};
}

#endif
