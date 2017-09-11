#ifndef GEAROENIX_VULKAN_MEMORY_MEMORY_Hpp
#define GEAROENIX_VULKAN_MEMORY_MEMORY_Hpp
#include <memory>
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
namespace image {
class Image;
}
namespace memory {
class Pool;
class Memory
{
private:
    std::shared_ptr<Pool> pool;
    VkDeviceMemory vulkan_data;
public:
    Memory(const std::shared_ptr<Pool> &pool, const VkMemoryRequirements &req, const VkMemoryPropertyFlagBits &properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ~Memory();
    const VkDeviceMemory &get_vulkan_data() const;
};
} // namespace memory
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_MEMORY_POOL_H
