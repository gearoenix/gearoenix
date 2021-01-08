#include "gx-vk-buf-dynamic.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-buf-manager.hpp"

gearoenix::vulkan::buffer::Dynamic::Dynamic(const std::size_t size, engine::Engine* const e) noexcept
    : size(size)
    , e(e)
{
}

gearoenix::vulkan::buffer::Dynamic::~Dynamic() noexcept = default;

#endif
