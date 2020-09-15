#include "gx-cr-asset-manager.hpp"
#include "../../audio/gx-au-manager.hpp"
#include "../../physics/constraint/gx-phs-cns-manager.hpp"
#include "../../render/camera/gx-rnd-cmr-manager.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../render/font/gx-rnd-fnt-manager.hpp"
#include "../../render/light/gx-rnd-lt-manager.hpp"
#include "../../render/mesh/gx-rnd-msh-manager.hpp"
#include "../../render/model/gx-rnd-mdl-manager.hpp"
#include "../../render/reflection/gx-rnd-rfl-manager.hpp"
#include "../../render/scene/gx-rnd-scn-manager.hpp"
#include "../../render/skybox/gx-rnd-sky-manager.hpp"
#include "../../render/texture/gx-rnd-txt-manager.hpp"
#include "../../system/gx-sys-app.hpp"
#include "../../system/stream/gx-sys-stm-asset.hpp"

gearoenix::core::asset::Manager::Manager(system::Application* const sys_app, const std::string& name) noexcept
    : sys_app(sys_app)
    , render_engine(sys_app->get_render_engine())
    , file(system::stream::Asset::construct(sys_app, name))
{
    if (file == nullptr) {
#define GX_HELPER(a, n) a##_manager = std::make_unique<n::Manager>(nullptr, render_engine);

        GX_HELPER(camera, render::camera)
        GX_HELPER(audio, audio)
        GX_HELPER(light, render::light)
        GX_HELPER(texture, render::texture)
        GX_HELPER(font, render::font)
        GX_HELPER(mesh, render::mesh)
        GX_HELPER(model, render::model)
        GX_HELPER(reflection, render::reflection)
        GX_HELPER(skybox, render::skybox)
        GX_HELPER(constraint, physics::constraint)
        GX_HELPER(scene, render::scene)
#undef GX_HELPER
    } else {
        GXLOGD("Asset file found.")
        auto* s = reinterpret_cast<system::stream::Stream*>(file.get());
        last_id = s->read<Id>();
        core::Count off;

#define GX_HELPER(a, n)                                  \
    off = s->tell();                                     \
    s = system::stream::Asset::construct(sys_app, name); \
    s->seek(off);                                        \
    a##_manager = std::make_unique<n::Manager>(std::unique_ptr<system::stream::Stream>(s), render_engine);

        GX_HELPER(camera, render::camera)
        GX_HELPER(audio, audio)
        GX_HELPER(light, render::light)
        GX_HELPER(texture, render::texture)
        GX_HELPER(font, render::font)
        GX_HELPER(mesh, render::mesh)
        GX_HELPER(model, render::model)
        GX_HELPER(reflection, render::reflection)
        GX_HELPER(skybox, render::skybox)
        GX_HELPER(constraint, physics::constraint)
        GX_HELPER(scene, render::scene)
#undef GX_HELPER
    }
}

gearoenix::core::asset::Manager::~Manager() noexcept
{
    GXLOGD("Asset manager deleted.")
}

#ifdef GX_THREAD_NOT_SUPPORTED
gearoenix::core::Id gearoenix::core::asset::Manager::last_id = 0;

gearoenix::core::Id gearoenix::core::asset::Manager::create_id() noexcept
{
    return last_id++;
}
#else
std::atomic<gearoenix::core::Id> gearoenix::core::asset::Manager::last_id(0);

gearoenix::core::Id gearoenix::core::asset::Manager::create_id() noexcept
{
    return last_id.fetch_add(1);
}
#endif
