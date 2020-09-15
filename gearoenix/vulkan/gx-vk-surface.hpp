#ifndef GEAROENIX_VULKAN_SURFACE_HPP
#define GEAROENIX_VULKAN_SURFACE_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::system {
class Application;
}

namespace gearoenix::vulkan {
class Instance;
class Surface {
private:
    const std::shared_ptr<Instance> instance;
    const system::Application* const sys_app;
    VkSurfaceKHR vulkan_data = nullptr;

public:
    Surface(std::shared_ptr<Instance> instance, const system::Application* sys_app) noexcept;
    ~Surface() noexcept;
    [[nodiscard]] const std::shared_ptr<Instance>& get_instance() const noexcept;
    [[nodiscard]] VkSurfaceKHR get_vulkan_data() const noexcept;
    [[nodiscard]] const system::Application* get_sys_app() const noexcept;
};
}
#endif
#endif
