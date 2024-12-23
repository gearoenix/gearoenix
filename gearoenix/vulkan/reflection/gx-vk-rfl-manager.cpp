#include "gx-vk-rfl-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"

std::shared_ptr<gearoenix::render::reflection::Builder> gearoenix::vulkan::reflection::Manager::build_baked(
    const std::string& name,
    const std::shared_ptr<render::texture::TextureCube>& irradiance,
    const std::shared_ptr<render::texture::TextureCube>& radiance,
    const math::Aabb3<double>& include_box,
    const core::job::EndCaller& end_callback)
{
    GX_UNIMPLEMENTED;
    return {};
}

std::shared_ptr<gearoenix::render::reflection::Builder> gearoenix::vulkan::reflection::Manager::build_runtime(
    const std::string& name,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::uint64_t environment_resolution,
    const std::uint64_t irradiance_resolution,
    const std::uint64_t radiance_resolution,
    const core::job::EndCaller& end_callback)
{
    GX_UNIMPLEMENTED;
    return {};
}

void gearoenix::vulkan::reflection::Manager::update()
{
    render::reflection::Manager::update();
}

gearoenix::vulkan::reflection::Manager::Manager(engine::Engine& e)
    : render::reflection::Manager(e)
    , vk_e(e)
{
}

gearoenix::vulkan::reflection::Manager::~Manager() = default;

#endif
