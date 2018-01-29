#include "rnd-scn-ui.hpp"
#include "../../core/event/cr-evn-event.hpp"

gearoenix::render::scene::Ui::Ui(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Scene(Scene::SceneType::UI, f, e, c)
{
}

void gearoenix::render::scene::Ui::on_event(const core::event::Event& e)
{
    Scene::on_event(e);
    //    switch(e.get_type())
    //    {
    //    case core::event::Event::
    //    }
    //    cameras[curcam]->
}
