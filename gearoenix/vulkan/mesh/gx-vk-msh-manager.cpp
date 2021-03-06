#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-msh-accel-manager.hpp"
#include "gx-vk-msh-raster-manager.hpp"

gearoenix::vulkan::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

std::shared_ptr<gearoenix::vulkan::mesh::Manager> gearoenix::vulkan::mesh::Manager::construct(
    engine::Engine& e) noexcept
{
    if (e.get_physical_device().get_rtx_supported())
        return std::make_shared<AccelManager>(e);
    else
        return std::make_shared<RasterManager>(e);
}

gearoenix::vulkan::mesh::Manager::~Manager() noexcept = default;

#endif