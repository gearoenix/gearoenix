#ifndef GEAROENIX_VULKAN_FENCE_HPP
#define GEAROENIX_VULKAN_FENCE_HPP
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace sync {
        class Fence {
        private:
            std::shared_ptr<device::Logical> logical_device;
            VkFence vulkan_data;

        public:
            Fence(const std::shared_ptr<device::Logical>& logical_device,
                bool signaled = false);
            ~Fence();
            void wait();
            const VkFence& get_vulkan_data() const;
        };
    }
}
}
#endif
