#include "rnd-sky-skybox.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../material/rnd-mat-skybox-basic.hpp"
#include "../rnd-engine.hpp"

gearoenix::render::skybox::Skybox::Skybox(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : mesh_id(s->read<core::Id>())
    , mat(static_cast<material::SkyboxBasic*>(material::Material::read(s, e, c)))
{
    msh = e->get_system_application()->get_asset_manager()->get_mesh(mesh_id, core::EndCaller<mesh::Mesh>([c](std::shared_ptr<mesh::Mesh>) -> void {}));
}

gearoenix::render::skybox::Skybox::~Skybox()
{
    delete mat;
}

gearoenix::render::skybox::Skybox* gearoenix::render::skybox::Skybox::read(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    switch (s->read<core::Id>()) {
    case 1:
        return new Skybox(s, e, c);
        break;
    default:
        UNEXPECTED;
    }
}
