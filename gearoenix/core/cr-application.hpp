#ifndef GEAROENIX_CORE_APPLICATION_HPP
#define GEAROENIX_CORE_APPLICATION_HPP
#include <memory>
namespace gearoenix {
namespace system {
class Application;
}
namespace core {
class Application {
private:
	//system::Application *sys_app;
public:
	Application(system::Application *sys_app);
	~Application();
    void update();
	void terminate();
};
}
}
#endif
