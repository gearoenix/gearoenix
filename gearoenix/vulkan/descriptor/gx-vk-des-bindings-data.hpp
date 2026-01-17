#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <map>
#include <memory>
#include <optional>
#include <thread>
#include <variant>
#include <vector>

namespace gearoenix::vulkan::descriptor {
struct Manager;
struct PoolManager;
struct Set;
struct SetLayout;
struct BindingsData final {
    friend struct Manager;

    GX_GET_REFC_PRV(std::shared_ptr<SetLayout>, layout);

    const std::vector<VkDescriptorPoolSize> pool_sizes;
    std::map<std::variant<std::uint64_t, std::thread::id>, std::weak_ptr<PoolManager>> pools;

    [[nodiscard]] static std::vector<VkDescriptorPoolSize> create_pool_sizes(const std::vector<VkDescriptorSetLayoutBinding>& data);
    explicit BindingsData(const std::vector<VkDescriptorSetLayoutBinding>& data);
    [[nodiscard]] std::shared_ptr<Set> create_set(std::optional<std::uint64_t> kernel_index);

public:
    ~BindingsData();
};
}

#endif