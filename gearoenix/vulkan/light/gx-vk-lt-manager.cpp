#include "gx-vk-lt-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "../../render/light/gx-rnd-lt-directional.hpp"
#include "../../render/light/gx-rnd-lt-point.hpp"

gearoenix::vulkan::light::Manager::Manager()
    : Singleton<Manager>(this)
    , directionals_uniform_indexer(render::light::Directional::max_count)
    , points_uniform_indexer(render::light::Point::max_count)
    , directional_shadow_casters_uniform_indexer(render::light::ShadowCasterDirectional::max_count)
{
}

gearoenix::vulkan::light::Manager::~Manager() = default;

void gearoenix::vulkan::light::Manager::update()
{
    directionals_uniform_indexer.reset();
    points_uniform_indexer.reset();
    directional_shadow_casters_uniform_indexer.reset();
    render::light::Manager::update();
}

void gearoenix::vulkan::light::Manager::upload_uniforms()
{
    directionals_uniform_indexer.update();
    points_uniform_indexer.update();
    directional_shadow_casters_uniform_indexer.update();
}

#endif
