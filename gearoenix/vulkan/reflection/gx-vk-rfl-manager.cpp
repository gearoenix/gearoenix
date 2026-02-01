#include "gx-vk-rfl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "../../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"

void gearoenix::vulkan::reflection::Manager::update()
{
    render::reflection::Manager::update();
}

gearoenix::vulkan::reflection::Manager::Manager()
    : Singleton<Manager>(this)
    , uniform_indexer(render::reflection::Baked::max_count + render::reflection::Runtime::max_count)
{
}

gearoenix::vulkan::reflection::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::reflection::Manager::build_baked(
    std::string&& name, core::ecs::Entity* const parent,
    std::shared_ptr<render::texture::TextureCube>&& irradiance,
    std::shared_ptr<render::texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box)
{
}

void gearoenix::vulkan::reflection::Manager::build_runtime(
    std::string&& name, core::ecs::Entity* const parent,
    const math::Aabb3<double>& receive_box, const math::Aabb3<double>& exclude_box, const math::Aabb3<double>& include_box,
    const std::uint32_t environment_resolution, const std::uint32_t irradiance_resolution, const std::uint32_t radiance_resolution,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
}

#endif
