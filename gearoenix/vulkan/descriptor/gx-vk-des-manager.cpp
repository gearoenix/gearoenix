#include "gx-vk-des-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
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
    std::variant<std::size_t, std::thread::id> ti;
    if (kernel_index.has_value())
        ti = *kernel_index;
    else
        ti = std::this_thread::get_id();
    auto bd = bindings_data[bindings][ti].lock();
    if (nullptr == bd) {
        bd = std::shared_ptr<BindingsData>(new BindingsData(logical_device, bindings));
        bindings_data[bindings][ti] = bd;
    }
    return bd->create_set();
}

#endif
