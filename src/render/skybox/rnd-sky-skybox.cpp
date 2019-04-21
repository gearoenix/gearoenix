//#include "rnd-sky-skybox.hpp"
//#include "../../core/asset/cr-asset-manager.hpp"
//#include "../../system/stream/sys-stm-stream.hpp"
//#include "../../system/sys-app.hpp"
//#include "../../system/sys-log.hpp"
//#include "../material/rnd-mat-skybox-basic.hpp"
//#include "../mesh/rnd-msh-mesh.hpp"
//#include "../rnd-engine.hpp"
//
//gearoenix::render::skybox::Skybox::Skybox(core::Id my_id, system::stream::Stream* s, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//    : core::asset::Asset(my_id, core::asset::Asset::AssetType::SKYBOX)
//    , mesh_id(s->read<core::Id>())
//    , mat(static_cast<material::SkyboxBasic*>(material::Material::read(s, e, c)))
//{
//    msh = e->get_system_application()->get_asset_manager()->get_mesh(mesh_id, core::sync::EndCaller<mesh::Mesh>([c](std::shared_ptr<mesh::Mesh>) -> void {}));
//}
//
//gearoenix::render::skybox::Skybox::~Skybox()
//{
//    delete mat;
//}
//
//gearoenix::render::skybox::Skybox* gearoenix::render::skybox::Skybox::read(core::Id my_id, system::stream::Stream* s, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//{
//    switch (s->read<core::Id>()) {
//    case 1:
//        return new Skybox(my_id, s, e, c);
//        break;
//    default:
//        GXUNEXPECTED;
//    }
//}
//
//void gearoenix::render::skybox::Skybox::draw()
//{
//    msh->bind();
//    mat->bind(nullptr);
//    msh->draw();
//}
//
//void gearoenix::render::skybox::Skybox::update(const scene::Scene* s)
//{
//    mat->update(s, nullptr);
//}
