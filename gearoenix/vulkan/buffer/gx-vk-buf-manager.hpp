#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../render/buffer/gx-rnd-buf-manager.hpp"
#include "../gx-vk-build-configuration.hpp"
#include "../gx-vk-loader.hpp"

#include <array>
#include <memory>

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform;
struct Manager final : render::buffer::Manager, core::Singleton<Manager> {
private:
    const bool is_unified_memory;
    const std::shared_ptr<Buffer> device_root;
    const std::shared_ptr<Buffer> host_root;
    const std::array<std::shared_ptr<Buffer>, frames_in_flight> each_frame;
    const std::shared_ptr<Buffer> each_frame_destination;

    [[nodiscard]] std::shared_ptr<Buffer> create_host_root() const;
    [[nodiscard]] std::shared_ptr<Buffer> create_device_root() const;
    [[nodiscard]] std::array<std::shared_ptr<Buffer>, frames_in_flight> create_each_frame() const;
    [[nodiscard]] std::shared_ptr<Buffer> create_each_frame_destination() const;

public:
    Manager();
    ~Manager() override;

    [[nodiscard]] const std::shared_ptr<Buffer>& get_device_root() const { return device_root; }

    [[nodiscard]] std::shared_ptr<Buffer> create_static(std::int64_t size);
    [[nodiscard]] std::shared_ptr<Buffer> create_staging(std::int64_t size);
    [[nodiscard]] std::shared_ptr<Uniform> create_uniform_vk(std::int64_t size);
    [[nodiscard]] std::shared_ptr<render::buffer::Uniform> create_uniform(std::size_t size) override;
    [[nodiscard]] std::shared_ptr<Buffer> create(const std::string& name, const void* data, std::int64_t size, core::job::EndCaller<>&& end);

    template <typename T>
    [[nodiscard]] std::shared_ptr<Buffer> create(const std::string& name, const std::vector<T>& data, core::job::EndCaller<>&& end)
    {
        return create(name, data.data(), data.size() * sizeof(T), std::move(end));
    }

    void upload_dynamics(vk::CommandBuffer vk_cmd);
};
}

#endif
