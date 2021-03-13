#ifndef GEAROENIX_VULKAN_DESCRIPTOR_SET_HPP
#define GEAROENIX_VULKAN_DESCRIPTOR_SET_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::descriptor {
struct SetLayout;
struct Pool;
struct PoolManager;
struct Set final {
    friend struct PoolManager;

    GX_GET_VAL_PRV(VkDescriptorSet, vulkan_data, nullptr)

private:
    Pool* const pool;
    const std::shared_ptr<PoolManager> pool_manager;

    Set(Pool*, std::shared_ptr<PoolManager> pool_manager, VkDescriptorSet vulkan_data) noexcept;
    [[nodiscard]] static std::shared_ptr<Set> construct(
        Pool* pool, std::shared_ptr<PoolManager> pool_manager, VkDescriptorSetLayout layout) noexcept;

public:
    //    Set(Pool* pool, SetLayout* lay, const VkDescriptorBufferInfo& buff_info, const VkDescriptorImageInfo& img_info);
    ~Set() noexcept;
    //    const Pool* get_pool() const;
    //    Pool* get_pool();
    //    unsigned int get_layouts_count() const;
    //    const VkDescriptorSetLayout& get_layout() const;
    //    const VkDescriptorSet& get_vulkan_data() const;
    //    void bind(pipeline::Pipeline* p, command::Buffer* c);
};
}
#endif
#endif
