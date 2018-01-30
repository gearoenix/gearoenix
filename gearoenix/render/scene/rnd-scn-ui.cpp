#include "rnd-scn-ui.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-bt-mouse.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../model/rnd-mdl-model.hpp"

gearoenix::render::scene::Ui::Ui(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Scene(Scene::SceneType::UI, f, e, c)
{
}

void gearoenix::render::scene::Ui::on_event(const core::event::Event& e)
{
    Scene::on_event(e);
    switch(e.get_type())
    {
	case core::event::Event::EventType::BUTTON:
	{
		const core::event::button::Button& be = e.to_button();
		switch (be.get_type())
		{
		case core::event::button::Button::ButtonType::MOUSE:
		{
			const core::event::button::Mouse& mbe = be.to_mouse();
			switch (be.get_key())
			{
			case core::event::button::Button::KeyType::LEFT:
				switch (be.get_action())
				{
				case core::event::button::Button::ActionType::PRESS:
				{
					math::Ray3 r =  cameras[curcam]->create_ray3(mbe.get_x(), mbe.get_y());
					core::Real tmin = std::numeric_limits<core::Real>::max();
					core::Id hitm = 0;
					for (const std::pair<core::Id, std::shared_ptr<model::Model>>& pidm : root_models) {
						const std::shared_ptr<model::Model>& m = pidm.second;
						if (m->get_type() != model::Model::ModelType::WIDGET) continue;
						if(m->hit(r, tmin)) hitm = pidm.first;
					}
					GXLOGI("hited model is: " << hitm << ", with distance: " << tmin);
					break;
				}
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
    }
}
