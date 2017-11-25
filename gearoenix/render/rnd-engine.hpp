#ifndef GEAROENIX_RENDER_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_HPP
#include "../core/cr-types.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
namespace gearoenix {
namespace core {
    class EndCaller;
    class Semaphore;
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
    private:
    protected:
        system::Application* sysapp;
        pipeline::Manager* pipmgr;
        std::mutex load_functions_mutex;
        std::vector<std::function<void()>> load_functions;
        std::mutex loaded_scenes_mutex;
        std::vector<std::shared_ptr<scene::Scene>> loaded_scenes;
        std::mutex scene_loader_mutex;
        std::vector<std::function<void()>> scene_loader_functions;
        core::Semaphore* scene_loader_signaler;
        std::thread scene_loader;
        volatile bool scene_loader_continue = true;

        void scene_loader_function();

    public:
        Engine(system::Application* system_application);
        virtual ~Engine();
        virtual void window_changed() = 0;
        virtual void update() = 0;
        virtual void terminate() = 0;
        virtual texture::Texture2D* create_texture_2d(system::File* file, std::shared_ptr<core::EndCaller> c) = 0;
        virtual texture::Cube* create_texture_cube(system::File* file, std::shared_ptr<core::EndCaller> c) = 0;
        virtual buffer::Mesh* create_mesh(unsigned int vec, system::File* file, std::shared_ptr<core::EndCaller> c) = 0;
        virtual buffer::Uniform* create_uniform(unsigned int s, std::shared_ptr<core::EndCaller> c) = 0;
        virtual shader::Shader* create_shader(core::Id sid, system::File* file, std::shared_ptr<core::EndCaller> c) = 0;
        virtual shader::Resources* create_shader_resources(core::Id sid, pipeline::Pipeline* p, buffer::Uniform* ub, std::shared_ptr<core::EndCaller> c) = 0;
        virtual pipeline::Pipeline* create_pipeline(core::Id sid, std::shared_ptr<core::EndCaller> c) = 0;
        const pipeline::Manager* get_pipeline_manager() const;
        pipeline::Manager* get_pipeline_manager();
        void add_load_function(std::function<void()> fun);
        const system::Application* get_system_application() const;
        system::Application* get_system_application();
        const std::shared_ptr<scene::Scene>& get_scene(unsigned int scene_index) const;
        void load_scene(core::Id scene_id, std::function<void(unsigned int)> on_load);
    };
}
}
#endif
