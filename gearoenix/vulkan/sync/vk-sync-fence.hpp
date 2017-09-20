#ifndef GEAROENIX_VULKAN_FENCE_HPP
#define GEAROENIX_VULKAN_FENCE_HPP
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace sync {
        class Fence {
        private:
            device::Logical* logical_device;
            VkFence vulkan_data;

        public:
            Fence(device::Logical* logical_device, bool signaled = false);
            ~Fence();
            void wait();
            const VkFence& get_vulkan_data() const;
        };
    }
}
}
#endif
