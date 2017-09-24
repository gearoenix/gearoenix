#ifndef GEAROENIX_VULKAN_ENGINE_HPP
#define GEAROENIX_VULKAN_ENGINE_HPP
#include <memory>
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
    namespace sync {
        class Fence;
        class Semaphore;
    }
    class Framebuffer;
    class Instance;
    class Linker;
    class RenderPass;
    class Surface;
    class Swapchain;
    class Engine {
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
        pipeline::Manager* pipmgr;
        void setup_draw_buffers();

    public:
        Engine(system::Application* sys_app);
        ~Engine();
        void window_changed();
        void update();
        void terminate();
        const device::Logical* get_logical_device() const;
        device::Logical* get_logical_device();
        RenderPass* get_render_pass();
        buffer::Manager* get_buffer_manager();
        system::Application* get_system_application();
    };
}
}
#endif
