#include "gx-vk-cmr-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::vulkan::camera::Manager::build(const std::string& name)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::vulkan::camera::Manager::update()
{
    render::camera::Manager::update();
}

gearoenix::vulkan::camera::Manager::Manager(engine::Engine& e)
    : render::camera::Manager(e)
    , vk_e(e)
{
}

gearoenix::vulkan::camera::Manager::~Manager() = default;

#endif
