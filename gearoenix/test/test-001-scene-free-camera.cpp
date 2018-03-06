#include "test-001-scene-free-camera.hpp"
#ifdef TEST001
#include "../core/event/cr-ev-bt-mouse.hpp"
#include "../core/event/cr-ev-mv-mouse.hpp"
#include "../core/event/cr-ev-sys-system.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/rnd-engine.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"

TestApp::TestApp(gearoenix::system::Application* sys_app)
    : gearoenix::core::Application::Application(sys_app)
    , rndeng(sys_app->get_render_engine())
{
    rndeng->load_scene(0, [this]() -> void {
        cam = rndeng->get_scene(0)->get_current_camera();
    });
    //    rndeng->load_scene(2, [this]() -> void {});
}

TestApp::~TestApp() {}

void TestApp::update() {}

void TestApp::terminate() {}

void TestApp::on_event(const gearoenix::core::event::Event& e)
{
    switch (e.get_type()) {
    case gearoenix::core::event::Event::From::SYSTEM: {
        const gearoenix::core::event::system::System& se = e.to_system();
        switch (se.get_action()) {
        case gearoenix::core::event::system::System::Action::RELOAD:
            rndeng->load_scene(0, [this]() -> void {
                cam = rndeng->get_scene(0)->get_current_camera();
                rndeng->load_scene(1, [this]() -> void {});
            });

            break;
        default:
            break;
        }
        break;
    }
    case gearoenix::core::event::Event::From::BUTTON: {
        const gearoenix::core::event::button::Button& be = e.to_button();
        switch (be.get_type()) {
        case gearoenix::core::event::button::Button::MOUSE: {
            switch (be.get_key()) {
            case gearoenix::core::event::button::Button::LEFT:
                switch (be.get_action()) {
                case gearoenix::core::event::button::Button::PRESS:
                    if (be.is_taken())
                        break;
                    left_mouse_down = true;
                    break;
                case gearoenix::core::event::button::Button::RELEASE:
                    left_mouse_down = false;
                    break;
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
    case gearoenix::core::event::Event::From::MOVEMENT: {
        const gearoenix::core::event::movement::Movement& me = e.to_movement();
        switch (me.get_type()) {
        case gearoenix::core::event::movement::Movement::MOUSE: {
            const gearoenix::core::event::movement::Mouse& mme = me.to_mouse();
            if (left_mouse_down) {
                cam->rotate_local_x(mme.get_dy() * 0.1f);
                cam->rotate_global_z(mme.get_dx() * 0.1f);
            }
            break;
        }
        default:
            break;
        }
    }
    default:
        break;
    }
}

GEAROENIX_START(TestApp)
#endif
