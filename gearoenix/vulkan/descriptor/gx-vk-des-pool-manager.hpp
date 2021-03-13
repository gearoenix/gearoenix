#ifndef GEAROENIX_RENDER_DESCRIPTOR_POOL_MANAGER_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_POOL_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>
#include <set>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Pool;
struct Set;
struct SetLayout;
struct BindingsData;
struct PoolManager final {
    friend struct BindingsData;
    friend struct Set;

private:
    GX_CREATE_GUARD(this)
    const device::Logical& logical_device;
    std::vector<std::unique_ptr<Pool>> pools;
    std::set<Pool*> free_pools;
    std::weak_ptr<PoolManager> self;

    explicit PoolManager(const device::Logical& logical_device) noexcept;
    [[nodiscard]] static std::shared_ptr<PoolManager> construct(const device::Logical& logical_device) noexcept;
    [[nodiscard]] std::shared_ptr<Set> create_set(
        const std::vector<VkDescriptorPoolSize>& pool_sizes,
        const SetLayout& layout) noexcept;
    void pool_freed(Pool*) noexcept;

public:
    ~PoolManager() noexcept;
};
}

#endif
#endif