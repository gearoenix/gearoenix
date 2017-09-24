#ifndef GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#define GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#include "../vk-linker.hpp"
#include <memory>
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
            std::shared_ptr<descriptor::Set> desset;
            VkPipelineLayout vulkan_data;

        public:
            Layout(const std::shared_ptr<descriptor::Set>& desset);
            ~Layout();
            const VkPipelineLayout& get_vulkan_data() const;
            const std::shared_ptr<descriptor::Set>& get_descriptor_set() const;
            const device::Logical* get_device() const;
        };
    } // namespace pipeline
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
