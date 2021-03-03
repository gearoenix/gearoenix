#ifndef GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#define GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::image {
struct Image;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform;
struct Manager final {
    GX_GET_RRF_PRV(memory::Manager, memory_manager)
    GX_GET_RRF_PRV(engine::Engine, e)

private:
    const std::shared_ptr<Buffer> upload_root_buffer;
    const std::shared_ptr<Buffer> gpu_root_buffer;
    const std::vector<std::shared_ptr<Buffer>> each_frame_upload_source;
    const std::shared_ptr<Buffer> each_frame_upload_destination;

    std::unique_ptr<core::sync::WorkWaiter> uploader;
    std::unique_ptr<core::sync::WorkWaiter> waiter;

    [[nodiscard]] std::shared_ptr<Buffer> create_upload_root_buffer() const noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> create_gpu_root_buffer() const noexcept;
    [[nodiscard]] std::vector<std::shared_ptr<Buffer>> create_each_frame_upload_source() const noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> create_each_frame_upload_destination() const noexcept;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(memory::Manager& memory_manager, engine::Engine& e) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> create_static(std::size_t size) noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> create_staging(std::size_t size) noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> create_dynamic(std::size_t size, std::size_t frame_number) noexcept;
    [[nodiscard]] std::shared_ptr<Uniform> create_uniform(std::size_t size) noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> create(
        const void* data,
        std::size_t size,
        core::sync::EndCaller<Buffer> end = GX_DEFAULT_END_CALLER(Buffer)) noexcept;

    template <typename T>
    [[nodiscard]] std::shared_ptr<Buffer> create(
        const std::vector<T>& data,
        core::sync::EndCaller<Buffer> end = GX_DEFAULT_END_CALLER(Buffer)) noexcept
    {
        return create(data.data(), data.size() * sizeof(T), std::move(end));
    }
};
}

#endif
#endif
