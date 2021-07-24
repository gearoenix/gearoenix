#ifndef GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
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
struct BindingsData;
struct Set;
struct Pool;
struct Manager final {
    GX_GET_UPTR_PRV(Pool, imgui)

private:
    const device::Logical& logical_device;
    std::map<std::vector<std::uint8_t>, std::unique_ptr<BindingsData>> bindings_data;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    explicit Manager(const device::Logical& logical_device) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] std::unique_ptr<BindingsData>& get_bindings_data(
        const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept;
    [[nodiscard]] std::shared_ptr<Set> create_set(
        const std::vector<VkDescriptorSetLayoutBinding>& bindings,
        std::optional<std::size_t> kernel_index = std::nullopt) noexcept;
};
}

#endif
#endif