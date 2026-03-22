#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/gx-cr-singleton.hpp"
#include "gx-vk-loader.hpp"

#include <memory>

#if GX_DEBUG_MODE
#define GX_VULKAN_INSTANCE_DEBUG true
#endif

namespace gearoenix::vulkan {
struct Instance final : core::Singleton<Instance> {
private:
    vk::raii::Context context;
    vk::raii::Instance vulkan_instance;
    std::uint32_t api_version = 0;
#if GX_VULKAN_INSTANCE_DEBUG
    vk::raii::DebugUtilsMessengerEXT debug_messenger;
#endif

    Instance();

public:
    Instance(Instance&&) = delete;
    Instance(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;
    Instance& operator=(const Instance&) = delete;
    ~Instance() override;

    [[nodiscard]] static std::unique_ptr<Instance> construct();
    [[nodiscard]] std::uint32_t get_api_version() const { return api_version; }
    [[nodiscard]] const vk::raii::Instance& get_instance() const { return vulkan_instance; }
    [[nodiscard]] vk::Instance get_vulkan_data() const { return *vulkan_instance; }
};
}
#endif
