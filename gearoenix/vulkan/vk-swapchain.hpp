#ifndef GEAROENIX_VULKAN_SWAPCHAIN_HPP
#define GEAROENIX_VULKAN_SWAPCHAIN_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "vk-linker.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace image {
        class View;
    }
    namespace sync {
        class Semaphore;
    }
    class Swapchain {
    private:
        device::Logical* logical_device;
        VkSurfaceFormatKHR chosen_format;
        VkSwapchainKHR vulkan_data = 0;
        std::vector<image::View*> image_views;

    public:
        Swapchain(device::Logical* d);
        ~Swapchain();
        const VkSwapchainKHR& get_vulkan_data() const;
        const device::Logical* get_logical_device() const;
        device::Logical* get_logical_device();
        const VkSurfaceFormatKHR& get_chosen_format() const;
        const std::vector<image::View*>& get_image_views() const;
        uint32_t get_next_image_index(const sync::Semaphore* semaphore);
        void initialize();
    };
}
}
#endif
#endif
