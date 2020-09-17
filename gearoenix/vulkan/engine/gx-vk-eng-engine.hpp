#ifndef GEAROENIX_VULKAN_ENGINE_HPP
#define GEAROENIX_VULKAN_ENGINE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN

#include "../../core/gx-cr-types.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace gearoenix::system {
class Application;
class File;
}

namespace gearoenix::vulkan::buffer {
class Manager;
}

namespace gearoenix::vulkan::command {
class Buffer;
class Pool;
}

namespace gearoenix::vulkan::device {
class Physical;
class Logical;
}

namespace gearoenix::vulkan::image {
class View;
}

namespace gearoenix::vulkan::memory {
class Manager;
}

namespace gearoenix::vulkan::pipeline {
class Manager;
}

namespace gearoenix::vulkan::scene {
class Scene;
}

namespace gearoenix::vulkan::sync {
class Fence;
class Semaphore;
}

namespace gearoenix::vulkan::texture {
class Sampler2D;
}

namespace gearoenix::vulkan {
class Framebuffer;
class Instance;
class RenderPass;
class Surface;
class Swapchain;
}

namespace gearoenix::vulkan::engine {
class Engine final : public render::engine::Engine {
private:
    system::Application* sys_app;
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Surface> surface;
    std::shared_ptr<device::Physical> physical_device;
    std::shared_ptr<device::Logical> logical_device;
    std::shared_ptr<Swapchain> swapchain;
    std::shared_ptr<image::View> depth_stencil;
    std::shared_ptr<RenderPass> render_pass;
    std::vector<std::shared_ptr<Framebuffer>> framebuffers;
    std::shared_ptr<command::Pool> graphic_cmd_pool;
    std::shared_ptr<sync::Semaphore> present_complete_semaphore;
    std::shared_ptr<sync::Semaphore> render_complete_semaphore;
    std::vector<std::shared_ptr<sync::Fence>> wait_fences;
    std::shared_ptr<memory::Manager> vmemmgr;
    std::shared_ptr<buffer::Manager> vbufmgr;
    std::vector<std::shared_ptr<buffer::Manager>> uvbufmgr;
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
    //    void setup_draw_buffers();

    explicit Engine(system::Application* sys_app) noexcept;

public:
    static Engine* construct(system::Application* sys_app) noexcept;
    ~Engine() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
    [[nodiscard]] std::shared_ptr<render::sync::Semaphore> create_semaphore() const noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_texture_2d(
        core::Id id,
        std::string name,
        std::vector<std::vector<std::uint8_t>> data,
        const render::texture::TextureInfo& info,
        std::size_t img_width,
        std::size_t img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::TextureCube> create_texture_cube(
        core::Id id,
        std::string name,
        std::vector<std::vector<std::vector<std::uint8_t>>> data,
        const render::texture::TextureInfo& info,
        std::size_t aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Target> create_render_target(
        core::Id id,
        const std::vector<render::texture::AttachmentInfo>& infos,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    void submit(
        std::size_t pres_count,
        const render::sync::Semaphore* const* pres,
        std::size_t cmds_count,
        const render::command::Buffer* const* cmds,
        std::size_t nxts_count,
        const render::sync::Semaphore* const* nxts) noexcept final;
    //    Engine(system::Application* sys_app);
    //    ~Engine();
    [[nodiscard]] static bool is_supported() noexcept;
    //    void window_changed();
    //    void update();
    //    void terminate();
    //    const Linker* get_linker() const;
    //    const device::Logical* get_logical_device() const;
    //    device::Logical* get_logical_device();
    //    RenderPass* get_render_pass();
    //    buffer::Manager* get_gpu_buffer_manager();
    //    buffer::Manager* get_cpu_buffer_manager();
    //    buffer::Manager* get_uniform_gpu_buffer_manager(unsigned int i);
    //    buffer::Manager* get_uniform_cpu_buffer_manager(unsigned int i);
    //    system::Application* get_system_application();
    //    memory::Manager* get_v_memory_manager();
    //    memory::Manager* get_cpu_memory_manager();
    //    const pipeline::Manager* get_pipeline_manager() const;
    //    pipeline::Manager* get_pipeline_manager();
    //    const texture::Sampler2D* get_sampler_2d() const;
    //    texture::Sampler2D* get_sampler_2d();
    //    unsigned int get_frames_count() const;
    //    unsigned int get_current_frame_index() const;
    //    command::Buffer* get_current_command_buffer();
    //    unsigned int load_scene(
    //        core::Id scene_id, std::function<void(unsigned int)> on_load = [](unsigned int) {});
    //    void push_todo(std::function<std::function<void()>(command::Buffer*)> fun);
};
}
#endif
#endif
