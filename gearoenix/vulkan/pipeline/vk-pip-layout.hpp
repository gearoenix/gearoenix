#ifndef GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#define GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace descriptor {
        class Set;
    }
    namespace pipeline {
        class Layout {
        private:
            device::Logical* dev;
            descriptor::Set* desset;
            VkPipelineLayout vulkan_data;

        public:
            Layout(descriptor::Set* desset);
            ~Layout();
            const VkPipelineLayout& get_vulkan_data() const;
            const descriptor::Set* get_descriptor_set() const;
            descriptor::Set* get_descriptor_set();
            const device::Logical* get_device() const;
        };
    } // namespace pipeline
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
