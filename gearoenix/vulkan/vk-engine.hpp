#ifndef GEAROENIX_NUFRAG_VULKAN_ENGINE_HPP
#define GEAROENIX_NUFRAG_VULKAN_ENGINE_HPP
#include <memory>
#include <vector>
namespace gearoenix {
namespace nufrag {
namespace core {
class Application;
}
namespace system {
class Application;
class File;
}
namespace render {
namespace buffer {
class Buffer;
class Uniform;
}
namespace command {
class Buffer;
class Pool;
}
namespace device {
class Physical;
class Logical;
}
namespace descriptor {
class Pool;
class Set;
}
namespace image {
class View;
}
namespace memory {
class Pool;
}
namespace pipeline {
class Cache;
class Layout;
class Pipeline;
}
namespace shader {
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
	system::Application *sys_app;
	std::shared_ptr<Linker> linker;
	std::shared_ptr<Instance> instance;
	std::shared_ptr<Surface> surface;
	std::shared_ptr<device::Physical> physical_device;
	std::shared_ptr<device::Logical> logical_device;
	std::shared_ptr<Swapchain> swapchain;
    std::shared_ptr<memory::Pool> mem_pool;
	std::shared_ptr<image::View> depth_stencil;
	std::shared_ptr<RenderPass> render_pass;
	std::vector<std::shared_ptr<Framebuffer> > framebuffers;
    std::shared_ptr<command::Pool> graphic_cmd_pool;
    std::shared_ptr<shader::Manager> shader_manager;
    // for this example
    std::shared_ptr<buffer::Buffer> mesh_buff;
    std::shared_ptr<buffer::Uniform> uniform;
    std::shared_ptr<pipeline::Layout> pipeline_layout;
    std::shared_ptr<pipeline::Cache> pipeline_cache;
    std::shared_ptr<pipeline::Pipeline> pipeline;
    std::shared_ptr<descriptor::Pool> descriptor_pool;
    std::shared_ptr<descriptor::Set> descriptor_set;
    std::shared_ptr<sync::Semaphore> present_complete_semaphore;
    std::shared_ptr<sync::Semaphore> render_complete_semaphore;
    std::vector<std::shared_ptr<sync::Fence> > wait_fences;
    std::vector<std::shared_ptr<command::Buffer> > draw_buffers;
    void setup_draw_buffers();
public:
	Engine(system::Application *sys_app);
	~Engine();
    void window_changed();
    void update();
    void terminate();
};
}
}
}
#endif
