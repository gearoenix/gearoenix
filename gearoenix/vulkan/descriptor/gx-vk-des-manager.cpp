#include "gx-vk-des-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-des-bindings-data.hpp"
#include "gx-vk-des-pool.hpp"
#include "gx-vk-des-set.hpp"

#include <boost/functional/hash.hpp>

#include <cstring>
#include <span>
#include <ranges>
#include <cstddef>

gearoenix::vulkan::descriptor::Manager::Manager() : Singleton(this)
    , imgui(Pool::create_imgui())
{
}

gearoenix::vulkan::descriptor::Manager::~Manager() = default;

std::unique_ptr<gearoenix::vulkan::descriptor::BindingsData>& gearoenix::vulkan::descriptor::Manager::get_bindings_data(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    const auto bytes = std::as_bytes(std::span{bindings});
    const auto* const begin = reinterpret_cast<const std::uint8_t *>(bytes.data());
    const auto* const end = begin + bytes.size();
    std::vector<std::uint8_t> key_val{begin, end};
    const auto key_hash = boost::hash_range(key_val.begin(), key_val.end());
    return bindings_data[bindless_key_t {key_hash, std::move(key_val)}];
}

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::Manager::create_set(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings,
    const std::optional<std::uint64_t> kernel_index)
{
    auto& bd = get_bindings_data(bindings);
    if (nullptr == bd) {
        bd = std::unique_ptr<BindingsData>(new BindingsData(bindings));
    }
    return bd->create_set(kernel_index);
}

#endif
