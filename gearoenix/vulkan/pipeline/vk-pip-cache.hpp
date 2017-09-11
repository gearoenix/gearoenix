#ifndef GEAROENIX_NUFRAG_VULKAN_PIPELINE_CACHE_HPP
#define GEAROENIX_NUFRAG_VULKAN_PIPELINE_CACHE_HPP
#include <memory>
#include "../vk-linker.hpp"
namespace gearoenix {
namespace nufrag {
namespace render {
namespace device {
class Logical;
}
namespace pipeline {
class Cache
{
private:
    std::shared_ptr<device::Logical> logical_device;
    VkPipelineCache vulkan_data;
public:
    Cache(const std::shared_ptr<device::Logical> &logical_device);
    ~Cache();
    const std::shared_ptr<device::Logical> &get_logical_device() const;
    const VkPipelineCache &get_vulkan_data() const;
};
} // namespace pipeline
} // namespace render
} // namespace nufrag
} // namespace gearoenix
#endif // GEAROENIX_NUFRAG_RENDER_PIPELINE_CACHE_HPP
