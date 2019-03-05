#ifndef GEAROENIX_VULKAN_ENGINE_HPP
#define GEAROENIX_VULKAN_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../core/cr-types.hpp"
#include "../render/rnd-engine.hpp"
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
namespace vulkan {
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
    class Engine : public render::Engine {
    private:
        system::Application* sys_app;
        Linker* linker;
        Instance* instance;
        Surface* surface;
        device::Physical* physical_device;
        device::Logical* logical_device;
        Swapchain* swapchain;
        image::View* depth_stencil;
        RenderPass* render_pass;
        std::vector<Framebuffer*> framebuffers;
        command::Pool* graphic_cmd_pool;
        sync::Semaphore* present_complete_semaphore;
        sync::Semaphore* render_complete_semaphore;
        std::vector<sync::Fence*> wait_fences;
        memory::Manager* vmemmgr;
        buffer::Manager* vbufmgr;
        std::vector<buffer::Manager*> uvbufmgr;
        memory::Manager* cmemmgr;
        buffer::Manager* cbufmgr;
        std::vector<buffer::Manager*> ucbufmgr;
        pipeline::Manager* pipmgr;
        texture::Sampler2D* sampler_2d;
        std::vector<command::Buffer*> cmd_bufs;
        std::mutex todos_mutex;
        std::vector<std::function<std::function<void()>(command::Buffer*)>> todos;
        std::vector<std::vector<std::function<void()>>> frames_cleanups;
        std::vector<std::shared_ptr<scene::Scene>> loaded_scenes;
        uint32_t current_frame = 9999999;
        uint32_t frames_count = 9999999;
        void setup_draw_buffers();

    public:
        Engine(system::Application* sys_app);
        ~Engine();
        static bool is_supported();
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
