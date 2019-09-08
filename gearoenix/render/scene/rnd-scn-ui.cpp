#include "rnd-scn-ui.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/animation/phs-anm-once.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include <limits>

void gearoenix::render::scene::Ui::init() noexcept
{
    auto cam = e->get_system_application()->get_asset_manager()->get_camera_manager()->create<camera::Orthographic>();
    add_camera(cam);
    cam->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, 50.0f));
    uniform.ambient_light = math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

gearoenix::render::scene::Ui::Ui(const core::Id my_id, system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Ui, f, e, c)
{
    init();
}

gearoenix::render::scene::Ui::Ui(const core::Id my_id, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Ui, e, c)
{
    init();
}

bool gearoenix::render::scene::Ui::on_event(const core::event::Data& d) noexcept
{
    switch (d.source) {
    case core::event::Id::ButtonMouse:
        const auto data = std::get<gearoenix::core::event::button::Data>(d.data);
        if (data.key == core::event::button::KeyId::Left && data.action == core::event::button::ActionId::Press) {
            const auto ray = cameras.begin()->second->create_ray3(data.x, data.y);
            auto h = hit(ray, std::numeric_limits<gearoenix::core::Real>::max());
            if (h.has_value()) {
                auto* cld = h.value().second;
                
                for (auto* mdl = cld->get_parent(); mdl != nullptr;  mdl = mdl->get_parent()) {
                    mdl->get_type
                }

                mdl->get_meshes().begin()->second->get_material()->set_color(cx, cy, cz, GxEndCallerIgnored([] {}));
                modal->set_enability(gearoenix::core::State::Set);
            }
        }
        break;
    }

    return false;
}
