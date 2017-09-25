#include "config.hpp"
#ifdef TEST001
#include "../core/cr-application.hpp"

class TestApp : public gearoenix::core::Application {
public:
    TestApp(gearoenix::system::Application* sys_app);
    ~TestApp();
    void update();
    void terminate();
};
#endif
