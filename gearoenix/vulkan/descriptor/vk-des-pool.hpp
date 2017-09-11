#ifndef GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace render {
namespace device {
class Logical;
}
namespace descriptor {
class Pool
{
private:
    std::shared_ptr<device::Logical> logical_device;
    VkDescriptorPool vulkan_data;
public:
    Pool(const std::shared_ptr<device::Logical> &logical_device);
    ~Pool();
    const std::shared_ptr<device::Logical> &get_logical_device() const;
    const VkDescriptorPool &get_vulkan_data() const;
};
} // namespace descriptor
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
