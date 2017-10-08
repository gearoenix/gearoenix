#ifndef GEAROENIX_VULKAN_DESCRIPTOR_SET_HPP
#define GEAROENIX_VULKAN_DESCRIPTOR_SET_HPP
#include "../../core/cache/cr-cache-cached.hpp"
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace descriptor {
        class Pool;
        class Set : public core::cache::Cached {
        private:
            Pool* pool;
            VkDescriptorSetLayout layout;
            unsigned int layouts_count;
            VkDescriptorSet vulkan_data;

        public:
            Set(Pool* pool, const VkDescriptorBufferInfo& buff_info);
            Set(Pool* pool, const VkDescriptorBufferInfo& buff_info, const VkDescriptorImageInfo& img_info);
            ~Set();
            const Pool* get_pool() const;
            Pool* get_pool();
            unsigned int get_layouts_count() const;
            const VkDescriptorSetLayout& get_layout() const;
            const VkDescriptorSet& get_vulkan_data() const;
        };

    } // namespace descriptor
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_DESCRIPTOR_SET_HPP
