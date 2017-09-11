#ifndef GEAROENIX_NUFRAG_VULKAN_DESCRIPTOR_SET_HPP
#define GEAROENIX_NUFRAG_VULKAN_DESCRIPTOR_SET_HPP
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace nufrag {
namespace render {
namespace buffer {
class Uniform;
}
namespace pipeline {
class Layout;
}
namespace descriptor {
class Pool;
class Set
{
private:
    std::shared_ptr<Pool> pool;
    std::shared_ptr<pipeline::Layout> pipeline_layout;
    std::shared_ptr<buffer::Uniform> uniform;
    VkDescriptorSet vulkan_data;
public:
    Set(
            const std::shared_ptr<Pool> &pool,
            const std::shared_ptr<pipeline::Layout> &pipeline_layout,
            const std::shared_ptr<buffer::Uniform> &uniform);
    ~Set();
    const VkDescriptorSet &get_vulkan_data() const;
};

} // namespace descriptor
} // namespace render
} // namespace nufrag
} // namespace gearoenix

#endif // GEAROENIX_NUFRAG_RENDER_DESCRIPTOR_SET_HPP
