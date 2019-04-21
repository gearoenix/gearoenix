#include "vk-pip-manager.hpp"
#ifdef USE_VULKAN
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../system/sys-app.hpp"
#include "../descriptor/vk-des-pool.hpp"
#include "../descriptor/vk-des-set-layout.hpp"
#include "../vk-engine.hpp"
#include "vk-pip-cache.hpp"
#include "vk-pip-pipeline.hpp"

gearoenix::render::pipeline::Manager::Manager(Engine* engine)
    : cache(new Cache(engine->get_logical_device()))
    , rndpass(engine->get_render_pass())
    , despool(new descriptor::Pool(engine->get_logical_device()))
    , cacher(new core::cache::Cacher())
    , engine(engine)
{
}

gearoenix::render::pipeline::Manager::~Manager()
{
    delete cacher;
    delete despool;
    delete cache;
}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::render::pipeline::Manager::get_pipeline(shader::Id sid)
{
    std::function<std::shared_ptr<Pipeline>()> fn_new = [this, sid] {
        return std::shared_ptr<Pipeline>(
            new Pipeline(
                sid, cache, rndpass,
                engine->get_system_application()->get_asset_manager()->get_shader(sid),
                new descriptor::SetLayout(engine->get_logical_device(), sid), engine));
    };
    return cacher->get(static_cast<core::Id>(sid), fn_new);
}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::render::pipeline::Manager::get_cached_pipeline(shader::Id sid)
{
    return cacher->get<Pipeline>(sid);
}

gearoenix::render::descriptor::Pool* gearoenix::render::pipeline::Manager::get_descriptor_pool()
{
    return despool;
}
#endif
