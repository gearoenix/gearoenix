#ifndef GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#define GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../render/shader/gx-rnd-shd-shader.hpp"
#include "../gx-vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        struct Logical;
    }
    namespace descriptor {
        struct SetLayout {
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
#endif
#endif // GEAROENIX_RENDER_DESCRIPTOR_SET_LAYOUT_HPP
