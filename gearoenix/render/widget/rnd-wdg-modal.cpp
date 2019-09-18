#include "rnd-wdg-modal.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/collider/phs-cld-aabb.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-dynamic.hpp"
#include "../scene/rnd-scn-ui.hpp"
#include "rnd-wdg-button.hpp"

gearoenix::render::widget::Modal::Modal(const core::Id my_id, system::stream::Stream* const s, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Modal, s, e, c)
{
}

gearoenix::render::widget::Modal::Modal(const core::Id my_id, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Modal, e, c)
{
    core::sync::EndCaller<Button> btncall([c](std::shared_ptr<Button>) {});
    core::sync::EndCaller<model::Dynamic> mdlcall([c](std::shared_ptr<model::Dynamic>) {});
    core::sync::EndCaller<mesh::Mesh> mshcall([c](std::shared_ptr<mesh::Mesh>) {});
    auto* astmgr = e->get_system_application()->get_asset_manager();
    auto& mdlmgr = astmgr->get_model_manager();
    auto& mshmgr = astmgr->get_mesh_manager();
    auto plate_mesh = mshmgr->create_plate(mshcall);

    std::vector<math::BasicVertex> close_vertices = {
        math::BasicVertex {
            math::Vec3(-1.0f, 1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.0f, 1.0f),
        },
        math::BasicVertex {
            math::Vec3(-0.8f, 1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.1f, 1.0f),
        },
        math::BasicVertex {
            math::Vec3(0.0f, 0.1f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.5f, 0.55f),
        },
        math::BasicVertex {
            math::Vec3(-0.1f, 0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.45f, 0.5f),
        },
        ////////////////////////////////////////////////////
        math::BasicVertex {
            math::Vec3(1.0f, 1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(1.0f, 1.0f),
        },
        math::BasicVertex {
            math::Vec3(0.8f, 1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.9f, 1.0f),
        },
        math::BasicVertex {
            math::Vec3(-0.8f, -1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.1f, 0.0f),
        },
        math::BasicVertex {
            math::Vec3(-1.0f, -1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.0f, 0.0f),
        },
        ////////////////////////////////////////////////////
        math::BasicVertex {
            math::Vec3(1.0f, -1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(1.0f, 0.0f),
        },
        math::BasicVertex {
            math::Vec3(0.8f, -1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.9f, 0.0f),
        },
        math::BasicVertex {
            math::Vec3(0.0f, -0.1f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.5f, 0.45f),
        },
        math::BasicVertex {
            math::Vec3(0.1f, 0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
            math::Vec2(0.55f, 0.5f),
        },
    };

    std::vector<std::uint32_t> close_indices = {
        0, 2, 1, 3, 2, 0,
        ////////////////////////////////////////
        5, 6, 4, 6, 5, 7,
        ////////////////////////////////////////
        11, 9, 8, 9, 11, 10
    };

    const core::Real close_ocr = 1.4f;

    auto close_mesh = mshmgr->create(std::move(close_vertices), std::move(close_indices), close_ocr, mshcall);

    {
        const std::shared_ptr<material::Material> mat(new material::Material(e, c));
        mat->set_color(0.02f, 0.02f, 0.02f, c);
        const auto mdl = mdlmgr->create<model::Dynamic>(mdlcall);
        mdl->add_mesh(std::make_shared<model::Mesh>(plate_mesh, mat));
        add_child(mdl);
    }
    {
        const std::shared_ptr<material::Material> mat(new material::Material(e, c));
        mat->set_color(0.9999f, 0.999f, 0.999f, c);
        close_mdl = mdlmgr->create<Button>(btncall);
        close_mdl->add_mesh(std::make_shared<model::Mesh>(close_mesh, mat));
        auto* tran = close_mdl->get_transformation();
        const core::Real scale = 0.05f;
        const math::Vec3 position(0.9f, 0.9f, 0.1f);
        tran->local_scale(scale);
        tran->set_location(position);
        add_child(close_mdl);
        close_mdl->set_on_click([this] { on_close(); });
    }
}

gearoenix::render::widget::Modal::~Modal() noexcept
{
}

void gearoenix::render::widget::Modal::set_scene(scene::Scene* const s) noexcept
{
    Widget::set_scene(s);
}

void gearoenix::render::widget::Modal::set_on_close(const std::function<void()>& f) noexcept
{
    on_close = f;
}
