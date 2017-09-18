#ifndef GEAROENIX_VULKAN_DEVICE_LOGICAL_HPP
#define GEAROENIX_VULKAN_DEVICE_LOGICAL_HPP
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace device {
        class Physical;
        class Logical {
        private:
            Physical* physical_device;
            VkDevice vulkan_data;
            VkQueue vk_graphic_queue;

        public:
            Logical(Physical* p);
            ~Logical();
            const Physical* get_physical_device() const;
            Physical* get_physical_device();
            const VkDevice& get_vulkan_data() const;
            const VkQueue& get_graphic_queue() const;
            void wait_to_finish();
        };
    }
}
}
#endif
