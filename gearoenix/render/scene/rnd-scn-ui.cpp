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

gearoenix::render::scene::Ui::Ui(const core::Id my_id, system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Ui, f, e, c)
{
}

gearoenix::render::scene::Ui::Ui(const core::Id my_id, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Ui, e, c)
{
    add_camera(e->get_system_application()->get_asset_manager()->get_camera_manager()->create<camera::Orthographic>());
}
