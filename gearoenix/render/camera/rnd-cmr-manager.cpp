#include "rnd-cmr-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-cmr-orthographic.hpp"
#include "rnd-cmr-perspective.hpp"

gearoenix::render::camera::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
    : e(e)
    , cache(s)
{
}

gearoenix::render::camera::Manager::~Manager()
{
}

std::shared_ptr<gearoenix::render::camera::Camera> gearoenix::render::camera::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Camera>& call)
{
    std::shared_ptr<Camera> data = cache.get<Camera>(id, [ this, id, call ]() noexcept->std::shared_ptr<Camera> {
        const std::shared_ptr<system::stream::Stream>& file = cache.get_file();
        const core::TypeId t = file->read<core::TypeId>();
        switch (t) {
        case 1:
            return std::shared_ptr<Perspective>(new Perspective(id, file, e));
        case 2:
            // return std::make_shared<Orthographic>(id, file, e);
            GXUNIMPLEMENTED;
        default:
            GXUNEXPECTED;
        }
    });
    call.set_data(data);
    return data;
}
