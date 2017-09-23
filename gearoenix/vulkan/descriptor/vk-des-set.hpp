#ifndef GEAROENIX_VULKAN_DESCRIPTOR_SET_HPP
#define GEAROENIX_VULKAN_DESCRIPTOR_SET_HPP
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace descriptor {
        class Pool;
        class Set {
        private:
            Pool* pool;
            VkDescriptorSetLayout layout;
            unsigned int layouts_count;
            VkDescriptorSet vulkan_data;

        public:
            Set(
                uint64_t shader_id,
                Pool* pool,
                const VkDescriptorBufferInfo& buff_info);
            ~Set();
            const Pool* get_pool() const;
            unsigned int get_layouts_count() const;
            const VkDescriptorSetLayout* get_layouts() const;
            const VkDescriptorSet& get_vulkan_data() const;
        };

    } // namespace descriptor
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_DESCRIPTOR_SET_HPP
