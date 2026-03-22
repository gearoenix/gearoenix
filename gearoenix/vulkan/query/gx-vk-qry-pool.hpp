#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <cstdint>
#include <map>
#include <mutex>

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::query {
struct Pool final {
private:
    vk::raii::QueryPool vulkan_data;

    std::mutex indices_lock;
    std::uint32_t latest_id = 0;
    std::map<vk::QueryType, std::map<std::uint64_t /*id*/, std::uint32_t>> indices;

    [[nodiscard]] std::uint32_t register_request(vk::QueryType, std::uint64_t id);

public:
    [[nodiscard]] vk::QueryPool get_vulkan_data() const { return *vulkan_data; }

    explicit Pool(vk::QueryType, std::uint32_t = 1);
    ~Pool();
    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    void issue_acceleration_structure_compacted_size(command::Buffer&, vk::AccelerationStructureKHR, std::uint64_t id = 0);
    [[nodiscard]] vk::DeviceSize get_acceleration_structure_compacted_size(std::uint64_t id = 0);
};
}

#endif
