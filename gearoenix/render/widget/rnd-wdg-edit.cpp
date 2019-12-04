#include "rnd-wdg-edit.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-dynamic.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-mesh.hpp"

std::wstring gearoenix::render::widget::Edit::cut_string(const std::wstring& t) const noexcept
{
    if (t.size() <= capacity)
        return t;
    if (cut_string_from_left)
        return t.substr(0, capacity);
    return t.substr(t.size() - capacity);
}

gearoenix::render::widget::Edit::Edit(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Text(my_id, f, e, c) {
        GXUNIMPLEMENTED
    }

    gearoenix::render::widget::Edit::Edit(const core::Id my_id, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Text(my_id, e, c)
{
    auto* const astmgr = e->get_system_application()->get_asset_manager();
    auto* const mshmgr = astmgr->get_mesh_manager();
    auto* const mdlmgr = astmgr->get_model_manager();

    auto mshend = core::sync::EndCaller<mesh::Mesh>([c](const std::shared_ptr<mesh::Mesh>&) {});
    const auto plate_mesh = mshmgr->create_plate(mshend);

    const std::shared_ptr<material::Material> bgmat(new material::Material(e, c));
    bgmat->set_color(0.02f, 0.02f, 0.02f, c);
    const auto bgmsh = std::make_shared<model::Mesh>(plate_mesh, bgmat);

    const std::shared_ptr<material::Material> curmat(new material::Material(e, c));
    curmat->set_color(0.0f, 0.0f, 0.0f, c);
    const auto curmsh = std::make_shared<model::Mesh>(plate_mesh, curmat);

    auto txtend = core::sync::EndCaller<Text>([c](const std::shared_ptr<Text>&) {});
    hint = mdlmgr->create<Text>(txtend);
    hint->set_text_color(0.3f, 0.3f, 0.3f, c);
    auto* const hint_tran = hint->get_transformation();
    hint_tran->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
    hint_tran->local_scale(0.95f);
    add_child(hint);

    auto mdlend = core::sync::EndCaller<model::Dynamic>([c](const std::shared_ptr<model::Dynamic>&) {});
    background = mdlmgr->create<model::Dynamic>(mdlend);
    background->add_mesh(bgmsh);
    background->get_transformation()->local_x_scale(9.0f);
    add_child(background);

    cursor = mdlmgr->create<model::Dynamic>(mdlend);
    cursor->add_mesh(curmsh);
    cursor->get_transformation()->local_x_scale(0.01f);
    add_child(cursor);
}

gearoenix::render::widget::Edit::~Edit() noexcept = default;

void gearoenix::render::widget::Edit::set_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    Text::set_text(cut_string(t), c);
}

void gearoenix::render::widget::Edit::set_hint_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    hint->set_text(cut_string(t), c);
}

void gearoenix::render::widget::Edit::set_capacity(const unsigned int c) noexcept
{
    background->get_transformation()->local_x_scale(static_cast<core::Real>(c) / static_cast<core::Real>(capacity));
    capacity = c;
}
