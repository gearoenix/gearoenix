#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-loader.hpp"

#include <flat_map>
#include <memory>
#include <optional>
#include <thread>
#include <variant>
#include <vector>

namespace gearoenix::vulkan::descriptor {
struct BindingsData;
struct Set;
struct Pool;
struct Manager final: core::Singleton<Manager> {
    using bindless_key_t = std::pair<std::size_t, std::vector<std::uint8_t>>;

    GX_GET_UPTR_PRV(Pool, imgui);

    std::flat_map<bindless_key_t, std::unique_ptr<BindingsData>> bindings_data;

public:
    Manager();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    ~Manager() override;
    [[nodiscard]] std::unique_ptr<BindingsData>& get_bindings_data(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    [[nodiscard]] std::shared_ptr<Set> create_set(const std::vector<VkDescriptorSetLayoutBinding>& bindings, std::optional<std::uint64_t> kernel_index = std::nullopt);
};
}

#endif