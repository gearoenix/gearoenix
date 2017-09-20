#ifndef GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#define GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace sync {
        class Semaphore {
        private:
            device::Logical* logical_device;
            VkSemaphore vulkan_data;

        public:
            Semaphore(device::Logical* logical_device);
            ~Semaphore();
            const device::Logical* get_logical_device() const;
            const VkSemaphore& get_vulkan_data() const;
        };
    } // namespace sync
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_SYNC_SEMAPHORE_HPP
