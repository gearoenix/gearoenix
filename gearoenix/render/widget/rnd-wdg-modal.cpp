#include "rnd-wdg-modal.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../../system/sys-app.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"

gearoenix::render::widget::Modal::Modal(const core::Id my_id, system::stream::Stream* const s, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Modal, s, e, c)
{
}

gearoenix::render::widget::Modal::Modal(const core::Id my_id, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Modal, e, c)
{
    core::sync::EndCaller<model::Model> mdlcall([c](std::shared_ptr<model::Model>) {});
    core::sync::EndCaller<mesh::Mesh> mshcall([c](std::shared_ptr<mesh::Mesh>) {});
    auto* astmgr = e->get_system_application()->get_asset_manager();
    auto &mdlmgr = astmgr->get_model_manager();
    auto& mshmgr = astmgr->get_mesh_manager();
    auto plate_mesh = mshmgr->create_plate(mshcall);
    {
        const std::shared_ptr<material::Material> mat(new material::Material(e, c));
        mat->set_roughness_factor(0.999f);
        mat->set_metallic_factor(0.001f);
        mat->set_color(0.999f, 0.999f, 0.999f, c);
        const auto mdl = mdlmgr->create<model::Model>(mdlcall);
        mdl->add_mesh(std::make_shared<model::Mesh>(plate_mesh, mat));
        mdl->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, -1.5f));
        add_child(mdl);
    }
}

gearoenix::render::widget::Modal::~Modal() noexcept
{
}
