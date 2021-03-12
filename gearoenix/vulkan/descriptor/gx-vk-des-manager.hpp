#ifndef GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-des-pool.hpp"
#include <map>
#include <memory>
#include <optional>
#include <thread>
#include <variant>
#include <vector>

namespace gearoenix::vulkan::descriptor {
struct BindingsData;
struct Set;
struct Manager final {
    GX_GET_REF_PRV(Pool, imgui)

private:
    const device::Logical& logical_device;
    std::map<std::vector<VkDescriptorSetLayoutBinding>, std::map<std::variant<std::size_t, std::thread::id>, std::weak_ptr<BindingsData>>> bindings_data;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    explicit Manager(const device::Logical& logical_device) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] std::shared_ptr<Set> create_set(
        const std::vector<VkDescriptorSetLayoutBinding>& bindings,
        std::optional<std::size_t> kernel_index = std::nullopt) noexcept;
};
}

#endif
#endif