#ifndef GEAROENIX_VULKAN_SURFACE_HPP
#define GEAROENIX_VULKAN_SURFACE_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../core/gx-cr-static.hpp"
#include "gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::system {
class Application;
}

namespace gearoenix::vulkan {
class Instance;
class Surface {
    GX_GET_REFC_PRV(std::shared_ptr<Instance>, instance)
    GX_GET_PTRC_PRV(system::Application, system_application);
    GX_GET_VAL_PRV(VkSurfaceKHR, vulkan_data, nullptr)

public:
    Surface(std::shared_ptr<Instance> instance, system::Application* sys_app) noexcept;
    ~Surface() noexcept;
};
}
#endif
#endif
