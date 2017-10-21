#ifndef GEAROENIX_OPENGL_ENGINE_HPP
#define GEAROENIX_OPENGL_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL
#include "../core/cr-types.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
namespace gearoenix {
namespace core {
    class Application;
}
namespace system {
    class Application;
    class File;
}
namespace render {
    namespace buffer {
        class Manager;
    }
    namespace command {
        class Buffer;
        class Pool;
    }
    namespace device {
        class Physical;
        class Logical;
    }
    namespace image {
        class View;
    }
    namespace memory {
        class Manager;
    }
    namespace pipeline {
        class Manager;
    }
    namespace scene {
        class Scene;
    }
    namespace sync {
        class Fence;
        class Semaphore;
    }
    namespace texture {
        class Sampler2D;
    }
    class Framebuffer;
    class Instance;
    class Linker;
    class RenderPass;
    class Surface;
    class Swapchain;
    class Engine {
    private:
    public:
        Engine(system::Application* sys_app);
        ~Engine();
        void window_changed();
        void update();
        void terminate();
        const Linker* get_linker() const;
        const device::Logical* get_logical_device() const;
        device::Logical* get_logical_device();
        RenderPass* get_render_pass();
        buffer::Manager* get_gpu_buffer_manager();
        buffer::Manager* get_cpu_buffer_manager();
        buffer::Manager* get_uniform_gpu_buffer_manager(unsigned int i);
        buffer::Manager* get_uniform_cpu_buffer_manager(unsigned int i);
        system::Application* get_system_application();
        memory::Manager* get_v_memory_manager();
        memory::Manager* get_cpu_memory_manager();
        const pipeline::Manager* get_pipeline_manager() const;
        pipeline::Manager* get_pipeline_manager();
        const texture::Sampler2D* get_sampler_2d() const;
        texture::Sampler2D* get_sampler_2d();
        unsigned int get_frames_count() const;
        unsigned int get_current_frame_index() const;
        command::Buffer* get_current_command_buffer();
        unsigned int load_scene(core::Id scene_id, std::function<void(unsigned int)> on_load = [](unsigned int) {});
        void push_todo(std::function<std::function<void()>(command::Buffer*)> fun);
    };
}
}
#endif
#endif
