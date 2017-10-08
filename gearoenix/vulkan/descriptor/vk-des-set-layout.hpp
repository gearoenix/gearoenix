#ifndef GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#define GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#include "../../render/shader/rnd-shd-shader.hpp"
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace descriptor {
        class SetLayout {
        private:
            device::Logical* dev;
            VkDescriptorSetLayout vulkan_data;

        public:
            SetLayout(device::Logical* dev, shader::Id sid);
            ~SetLayout();
            const device::Logical* get_logical_device() const;
            const VkDescriptorSetLayout& get_vulkan_data() const;
        };

    } // namespace descriptor
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_DESCRIPTOR_SET_LAYOUT_HPP
