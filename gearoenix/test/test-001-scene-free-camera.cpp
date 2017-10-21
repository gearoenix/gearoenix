#include "test-001-scene-free-camera.hpp"
#ifdef TEST001
#include "../render/rnd-engine.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"

TestApp::TestApp(gearoenix::system::Application* sys_app)
    : gearoenix::core::Application::Application(sys_app)
{
    //    sys_app->get_render_engine()->load_scene(0, [](unsigned int index) {
    //        LOGI(index << "Loaded.");
    //    });
}

TestApp::~TestApp() {}

void TestApp::update() {}

void TestApp::terminate() {}

GEAROENIX_START(TestApp)
#endif
