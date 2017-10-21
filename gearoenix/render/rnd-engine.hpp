#ifndef GEAROENIX_RENDER_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_HPP
namespace gearoenix {
namespace system {
    class Application;
    class File;
}
namespace render {
    namespace pipeline {
        class Manager;
    }
    class Engine {
    private:
    protected:
        system::Application* sysapp;
        pipeline::Manager* pipmgr;

    public:
        Engine(system::Application* system_application);
        virtual ~Engine();
        virtual void window_changed() = 0;
        virtual void update() = 0;
        virtual void terminate() = 0;
        const system::Application* get_system_application() const;
        system::Application* get_system_application();
        const pipeline::Manager* get_pipeline_manager() const;
        pipeline::Manager* get_pipeline_manager();
    };
}
}
#endif
