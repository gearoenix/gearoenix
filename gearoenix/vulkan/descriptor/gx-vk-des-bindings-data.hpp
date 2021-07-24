#ifndef GEAROENIX_RENDER_DESCRIPTOR_BINDINGS_DATA_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_BINDINGS_DATA_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <map>
#include <memory>
#include <optional>
#include <thread>
#include <variant>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Manager;
struct PoolManager;
struct Set;
struct SetLayout;
struct BindingsData final {
    friend struct Manager;

    GX_GET_REFC_PRV(std::shared_ptr<SetLayout>, layout)
private:
    const device::Logical& logical_device;
    const std::vector<VkDescriptorPoolSize> pool_sizes;
    std::map<std::variant<std::size_t, std::thread::id>, std::weak_ptr<PoolManager>> pools;

    [[nodiscard]] static std::vector<VkDescriptorPoolSize> create_pool_sizes(
        const std::vector<VkDescriptorSetLayoutBinding>& data) noexcept;
    BindingsData(const device::Logical& logical_device, const std::vector<VkDescriptorSetLayoutBinding>& data) noexcept;
    [[nodiscard]] std::shared_ptr<Set> create_set(std::optional<std::size_t> kernel_index) noexcept;

public:
    ~BindingsData() noexcept;
};
}

#endif
#endif