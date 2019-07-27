#include "rnd-scn-ui.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../physics/animation/phs-anm-once.hpp"
#include "../../physics/collider/phs-collider.hpp"
#include "../../physics/phs-engine.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include <limits>

gearoenix::render::scene::Ui::Ui(const core::Id my_id, system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
	: Scene(my_id, f, e, c)
{
}
