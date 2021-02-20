#ifndef GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#define GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "gx-vk-buf-uniform.hpp"

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
struct Manager final {
    GX_GET_RRF_PRV(memory::Manager, memory_manager)
    GX_GET_RRF_PRV(engine::Engine, e)

private:
    Buffer upload_root_buffer;
    Buffer gpu_root_buffer;
    std::vector<Buffer> each_frame_upload_source;
    Buffer each_frame_upload_destination;

    std::vector<std::pair<std::size_t /*destination offset*/, Buffer>> upload_buffers;
    std::vector<std::pair<Uniform, image::Image*>> copy_images;

    Manager(
        memory::Manager& memory_manager,
        engine::Engine& e,
        Buffer&& upload_root_buffer,
        Buffer&& gpu_root_buffer,
        std::vector<Buffer>&& each_frame_upload_source,
        Buffer&& each_frame_upload_destination) noexcept;

public:
    Manager(Manager&&) noexcept;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    [[nodiscard]] static Manager construct(memory::Manager& memory_manager, engine::Engine& e) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] Uniform create_uniform(std::size_t size, std::size_t frame_number) noexcept;
    [[nodiscard]] std::optional<Buffer> create(const void* data, std::size_t size) noexcept;
    template <typename T>
    [[nodiscard]] std::optional<Buffer> create(const std::vector<T>& data) noexcept;
};
}

template <typename T>
std::optional<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create(const std::vector<T>& data) noexcept
{
    return create(data.data(), data.size() * sizeof(T));
}

#endif
#endif
