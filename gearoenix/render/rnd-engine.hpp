#ifndef GEAROENIX_RENDER_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-types.hpp"
#include "../core/sync/cr-sync-end-caller.hpp"
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <thread>
#include <vector>
namespace gearoenix {
namespace core {
    namespace event {
        class Event;
    }
    namespace sync {
        class Semaphore;
    }
}
namespace physics {
    class Engine;
}
namespace system {
    class Application;
    namespace stream {
        class Stream;
    }
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
        typedef enum : core::Id {
            VULKAN,
            OPENGL_43,
            OPENGL_33,
            OPENGL_ES3,
            OPENGL_ES2,
            DIRECTX_11,
            DIRECTX_12,
            METAL,
        } EngineType;

    private:
    protected:
        EngineType engine_type = (EngineType)0xFFFFFFFFFFFFFFFF;
        system::Application* sysapp;
        pipeline::Manager* pipmgr;
        core::sync::Semaphore* load_functions_mutex;
        core::sync::Semaphore* loaded_scenes_mutex;
        core::sync::Semaphore* scene_loader_mutex;
        core::sync::Semaphore* scene_loader_signaler;
        std::thread scene_loader;
        volatile bool scene_loader_continue = true;
        void scene_loader_function();
        std::vector<std::function<void()>> scene_loader_functions;
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
        virtual texture::Texture2D* create_texture_2d(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c) = 0;
        virtual texture::Cube* create_texture_cube(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c) = 0;
        virtual buffer::Mesh* create_mesh(unsigned int vec, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c) = 0;
        virtual buffer::Uniform* create_uniform(unsigned int s, core::sync::EndCaller<core::sync::EndCallerIgnore> c) = 0;
        virtual shader::Shader* create_shader(core::Id sid, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c) = 0;
        virtual shader::Resources* create_shader_resources(core::Id sid, pipeline::Pipeline* p, buffer::Uniform* ub, core::sync::EndCaller<core::sync::EndCallerIgnore> c) = 0;
        virtual pipeline::Pipeline* create_pipeline(core::Id sid, core::sync::EndCaller<core::sync::EndCallerIgnore> c) = 0;
        virtual void on_event(core::event::Event& e);
        const pipeline::Manager* get_pipeline_manager() const;
        pipeline::Manager* get_pipeline_manager();
        void add_load_function(std::function<void()> fun);
        const system::Application* get_system_application() const;
        system::Application* get_system_application();
        const std::shared_ptr<scene::Scene>& get_scene(core::Id scene_id) const;
        const std::map<core::Id, std::shared_ptr<scene::Scene>>& get_all_scenes() const;
        void load_scene(core::Id scene_id, std::function<void()> on_load);
        void delete_scene(core::Id scene_id);
        physics::Engine* get_physics_engine();
    };
}
}
#endif
