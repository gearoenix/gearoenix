#ifndef GEAROENIX_VULKAN_SURFAE_HPP
#define GEAROENIX_VULKAN_SURFAE_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "gx-vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    class Application;
}
namespace render {
    class Instance;
    class Surface {
    private:
        Instance* instance;
        VkSurfaceKHR vulkan_data;
        system::Application* sys_app;

    public:
        Surface(Instance* instance,
            system::Application* sys_app);
        ~Surface();
        const Instance* get_instance() const;
        Instance* get_instance();
        const VkSurfaceKHR& get_vulkan_data() const;
        const system::Application* get_sys_app() const;
    };
}
}
#endif
#endif
