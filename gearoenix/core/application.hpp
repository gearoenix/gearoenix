#ifndef GEAROENIX_NUFRAG_CORE_APPLICATION_HPP
#define GEAROENIX_NUFRAG_CORE_APPLICATION_HPP
#include <memory>
namespace gearoenix {
namespace nufrag {
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
}
#endif
