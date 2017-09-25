#include "test-001-scene-free-camera.hpp"
#ifdef TEST001
#include "../system/sys-app.hpp"
TestApp::TestApp(gearoenix::system::Application* sys_app)
    : gearoenix::core::Application::Application(sys_app)
{
}

TestApp::~TestApp() {}

void TestApp::update() {}

void TestApp::terminate() {}

GEAROENIX_START(TestApp)
#endif
