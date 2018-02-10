#ifndef GEAROENIX_RENDER_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-end-caller.hpp"
#include "../core/cr-types.hpp"
#include <chrono>
#include <functional>
#include <memory>
#ifdef THREAD_SUPPORTED
#include <thread>
#endif
#include <map>
#include <set>
#include <vector>
namespace gearoenix {
namespace core {
    namespace event {
        class Event;
    }
#ifdef THREAD_SUPPORTED
    class Semaphore;
#endif
}
namespace physics {
    class Engine;
}
namespace system {
    class Application;
    class File;
}
namespace render {
    namespace buffer {
        class Mesh;
        class Uniform;
    }
    namespace pipeline {
        class Manager;
        class Pipeline;
    }
    namespace scene {
        class Scene;
    }
    namespace shader {
        class Shader;
        class Resources;
    }
    namespace texture {
        class Texture2D;
        class Cube;
    }
    class Engine {
    public:
        enum EngineType : core::Id {
            VULKAN,
            OPENGL_43,
            OPENGL_33,
            OPENGL_ES3,
            OPENGL_ES2,
            DIRECTX_11,
            DIRECTX_12,
            METAL,
            UNKNOWN,
        };

    private:
    protected:
        EngineType engine_type = EngineType::UNKNOWN;
        system::Application* sysapp;
        pipeline::Manager* pipmgr;
#ifdef THREAD_SUPPORTED
        core::Semaphore* load_functions_mutex;
        core::Semaphore* loaded_scenes_mutex;
        core::Semaphore* scene_loader_mutex;
        core::Semaphore* scene_loader_signaler;
        std::thread scene_loader;
        volatile bool scene_loader_continue = true;
        void scene_loader_function();
        std::vector<std::function<void()>> scene_loader_functions;
#endif
        std::vector<std::function<void()>> load_functions;
        std::map<core::Id, std::shared_ptr<scene::Scene>> loaded_scenes;
        physics::Engine* physics_engine = nullptr;
        void do_load_functions();
        void clear();
        Engine(system::Application* system_application);

    public:
        virtual ~Engine();
        virtual void window_changed() = 0;
        virtual void update() = 0;
        virtual void terminate() = 0;
        virtual texture::Texture2D* create_texture_2d(system::File* file, core::EndCaller<core::EndCallerIgnore> c) = 0;
        virtual texture::Cube* create_texture_cube(system::File* file, core::EndCaller<core::EndCallerIgnore> c) = 0;
        virtual buffer::Mesh* create_mesh(unsigned int vec, system::File* file, core::EndCaller<core::EndCallerIgnore> c) = 0;
        virtual buffer::Uniform* create_uniform(unsigned int s, core::EndCaller<core::EndCallerIgnore> c) = 0;
        virtual shader::Shader* create_shader(core::Id sid, system::File* file, core::EndCaller<core::EndCallerIgnore> c) = 0;
        virtual shader::Resources* create_shader_resources(core::Id sid, pipeline::Pipeline* p, buffer::Uniform* ub, core::EndCaller<core::EndCallerIgnore> c) = 0;
        virtual pipeline::Pipeline* create_pipeline(core::Id sid, core::EndCaller<core::EndCallerIgnore> c) = 0;
        virtual void on_event(core::event::Event& e);
        const pipeline::Manager* get_pipeline_manager() const;
        pipeline::Manager* get_pipeline_manager();
        void add_load_function(std::function<void()> fun);
        const system::Application* get_system_application() const;
        system::Application* get_system_application();
        const std::shared_ptr<scene::Scene>& get_scene(core::Id scene_id) const;
        const std::map<core::Id, std::shared_ptr<scene::Scene>>& get_all_scenes() const;
        void load_scene(core::Id scene_id, std::function<void()> on_load);
        physics::Engine* get_physics_engine();
    };
}
}
#endif
