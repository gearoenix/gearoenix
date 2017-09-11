#ifndef GEAROENIX_NUFRAG_VULKAN_SYNC_SEMAPHORE_HPP
#define GEAROENIX_NUFRAG_VULKAN_SYNC_SEMAPHORE_HPP
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace nufrag {
namespace render {
namespace device {
class Logical;
}
namespace sync {
class Semaphore
{
private:
    std::shared_ptr<device::Logical> logical_device;
    VkSemaphore vulkan_data;
public:
    Semaphore(const std::shared_ptr<device::Logical> &logical_device);
    ~Semaphore();
    const std::shared_ptr<device::Logical> &get_logical_device() const;
    const VkSemaphore &get_vulkan_data() const;
};
} // namespace sync
} // namespace render
} // namespace nufrag
} // namespace gearoenix
#endif // GEAROENIX_NUFRAG_RENDER_SYNC_SEMAPHORE_HPP
