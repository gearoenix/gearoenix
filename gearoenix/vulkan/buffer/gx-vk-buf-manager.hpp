#ifndef GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#define GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "gx-vk-buf-static.hpp"
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
    GX_GET_PTR_PRV(memory::Manager, memory_manager)
    GX_GET_PTRC_PRV(engine::Engine, e)
private:
    std::vector<Buffer> per_frame_cpu_root_buffers;
    Buffer upload_root_buffer;
    Buffer gpu_root_buffer;

    std::vector<std::pair<Uniform, Static*>> copy_buffers;
    std::vector<std::pair<Uniform, image::Image*>> copy_images;

    [[nodiscard]] std::vector<Buffer> create_per_frame_cpu_root_buffers() const noexcept;
    [[nodiscard]] Static create_static(std::size_t size, const void* data) noexcept;

public:
    Manager(memory::Manager* memory_manager, engine::Engine* e) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] Uniform create_uniform(std::size_t size, std::size_t frame_number) noexcept;
    template <typename T>
    [[nodiscard]] Static create_static(const std::vector<T>& data) noexcept;
};
}

template <typename T>
gearoenix::vulkan::buffer::Static gearoenix::vulkan::buffer::Manager::create_static(const std::vector<T>& data) noexcept
{
    return create_static(data.size() * sizeof(std::remove_reference<decltype(data)>::type::value_type), data.data());
}

#endif
#endif
