#ifndef GEAROENIX_RENDER_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_HPP
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace system {
    class Application;
    class File;
}
namespace render {
    namespace pipeline {
        class Manager;
    }
    namespace texture {
        class Texture2D;
    }
    class Engine {
    private:
    protected:
        system::Application* sysapp;
        pipeline::Manager* pipmgr;
        std::mutex load_functions_mutex;
        std::vector<std::function<void()>> load_functions;

    public:
        Engine(system::Application* system_application);
        virtual ~Engine();
        virtual void window_changed() = 0;
        virtual void update() = 0;
        virtual void terminate() = 0;
        virtual texture::Texture2D* create_texture_2d(system::File* file, std::shared_ptr<core::EndCaller> c) = 0;
        void add_load_function(std::function<void()> fun);
        const system::Application* get_system_application() const;
        system::Application* get_system_application();
        const pipeline::Manager* get_pipeline_manager() const;
        pipeline::Manager* get_pipeline_manager();
    };
}
}
#endif
