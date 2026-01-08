#include "gx-vk-mdl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::model::Manager::Manager()
    : Singleton<Manager>(this)
{
}

gearoenix::vulkan::model::Manager::~Manager() = default;

#endif
