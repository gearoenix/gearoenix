#ifndef GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#define GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace descriptor {
        class SetLayout;
    }
    namespace pipeline {
        class Layout {
        private:
            descriptor::SetLayout* dessetlay;
            VkPipelineLayout vulkan_data;

        public:
            Layout(descriptor::SetLayout* dessetlay);
            ~Layout();
            const VkPipelineLayout& get_vulkan_data() const;
            const descriptor::SetLayout* get_descriptor_set_layout() const;
        };
    } // namespace pipeline
} // namespace render
} // namespace gearoenix
#endif
#endif // GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
