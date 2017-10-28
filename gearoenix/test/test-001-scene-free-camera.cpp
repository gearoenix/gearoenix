#include "test-001-scene-free-camera.hpp"
#ifdef TEST001
#include "../render/rnd-engine.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"

TestApp::TestApp(gearoenix::system::Application* sys_app)
    : gearoenix::core::Application::Application(sys_app)
    , eng(sys_app->get_render_engine())
{
    sys_app->get_render_engine()->load_scene(0, [this] (unsigned int index) {
        cam = eng->get_scene(index)->get_current_camera();
        LOGI(index << "Loaded.");
    });
}

TestApp::~TestApp() {}

void TestApp::update() {}

void TestApp::terminate() {}

void TestApp::on_zoom(gearoenix::core::Real d)
{
    cam->move_forward(d);
}

void TestApp::on_rotate(gearoenix::core::Real d)
{
    cam->move_forward(d * 0.0001f);
}

GEAROENIX_START(TestApp)
#endif
