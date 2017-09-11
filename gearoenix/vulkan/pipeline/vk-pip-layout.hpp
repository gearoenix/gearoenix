#ifndef GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#define GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#include <memory>
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
namespace device {
class Logical;
}
namespace pipeline {
class Layout
{
private:
    std::shared_ptr<device::Logical> logical_device;
    VkDescriptorSetLayout descriptor_set_layout;
    VkPipelineLayout vulkan_data;
public:
    Layout(const std::shared_ptr<device::Logical> &logical_device);
    ~Layout();
    const VkPipelineLayout &get_vulkan_data() const;
    const VkDescriptorSetLayout &get_descriptor_set_layout() const;
};
} // namespace pipeline
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
