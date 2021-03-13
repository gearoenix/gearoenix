#include "gx-vk-des-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-des-bindings-data.hpp"
#include "gx-vk-des-pool.hpp"
#include "gx-vk-des-set.hpp"

gearoenix::vulkan::descriptor::Manager::Manager(const device::Logical& logical_device) noexcept
    : imgui(Pool::create_imgui(logical_device))
    , logical_device(logical_device)
{
}

gearoenix::vulkan::descriptor::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::Manager::create_set(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings,
    const std::optional<std::size_t> kernel_index) noexcept
{
    const auto bsz = bindings.size() * sizeof(bindings[0]);
    auto sz = bsz / sizeof(std::size_t);
    auto ksz = sz * sizeof(std::size_t);
    if (bsz != ksz) {
        ++sz;
        ksz += sizeof(std::size_t);
    }
    std::vector<std::size_t> key(sz);
    key.back() = 0;
    std::memcpy(key.data(), bindings.data(), bsz);
    auto& bd = bindings_data[key];
    if (nullptr == bd) {
        bd = std::unique_ptr<BindingsData>(new BindingsData(logical_device, bindings));
    }
    return bd->create_set(kernel_index);
}

#endif
