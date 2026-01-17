#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/engine/gx-rnd-eng-engine.hpp"

namespace gearoenix::vulkan {
struct Framebuffer;
struct ImGuiManager;
struct Instance;
struct RenderPass;
struct Surface;
struct Swapchain;
}

namespace gearoenix::vulkan::buffer {
struct Manager;
}

namespace gearoenix::vulkan::command {
struct Manager;
}

namespace gearoenix::vulkan::descriptor {
struct Manager;
struct Bindless;
}

namespace gearoenix::vulkan::device {
struct Physical;
struct Logical;
}

namespace gearoenix::vulkan::image {
struct View;
struct Manager;
}

namespace gearoenix::vulkan::texture {
struct Manager;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::mesh {
struct Manager;
}

namespace gearoenix::vulkan::pipeline {
struct Manager;
}

namespace gearoenix::vulkan::queue {
struct Graph;
}

namespace gearoenix::vulkan::engine {
struct Frame;
struct Engine final : render::engine::Engine, core::Singleton<Engine> {
    GX_GET_UPTR_PRV(Instance, instance);
    GX_GET_UPTR_PRV(Surface, surface);
    GX_GET_UPTR_PRV(device::Physical, physical_device);
    GX_GET_UPTR_PRV(device::Logical, logical_device);
    GX_GET_UPTR_PRV(Swapchain, swapchain);
    GX_GET_UPTR_PRV(memory::Manager, memory_manager);
    GX_GET_UPTR_PRV(command::Manager, command_manager);
    GX_GET_UPTR_PRV(descriptor::Manager, descriptor_manager);
    GX_GET_UPTR_PRV(descriptor::Bindless, bindless_descriptor_manager);
    GX_GET_UPTR_PRV(pipeline::Manager, pipeline_manager);
    GX_GET_UPTR_PRV(buffer::Manager, buffer_manager);
    GX_GET_CREF_PRV(std::shared_ptr<image::View>, depth_stencil);
    GX_GET_REFC_PRV(std::shared_ptr<RenderPass>, render_pass);
    GX_GET_UPTR_PRV(queue::Graph, graphed_queue);
    GX_GET_UPTR_PRV(ImGuiManager, imgui_manager);
    GX_GET_UPTR_PRV(image::Manager, vk_image_manager);
    GX_GET_PTRC_PRV(texture::Manager, vk_texture_manager);
    GX_GET_PTRC_PRV(mesh::Manager, vk_mesh_manager);
    GX_GET_CREF_PRV(std::vector<std::unique_ptr<Frame>>, frames);
    GX_GET_VAL_PRV(std::uint32_t, swapchain_image_index, 0);
    GX_GET_VAL_PRV(bool, swapchain_image_is_valid, true);
    //    GX_GET_CREF_PRV(std::shared_ptr<sampler::Manager>, sampler_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<image::Manager>, image_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<texture::MainTarget>, vulkan_main_render_target)
    //    GX_GET_CREF_PRV(std::vector<std::shared_ptr<command::Buffer>>, upload_command_buffers)
    //    GX_GET_CREF_PRV(std::vector<std::shared_ptr<sync::Semaphore>>, upload_semaphore)

    void initialize_frame();
    void window_resized() override;

public:
    Engine();
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    ~Engine() override;
    void start_frame() override;
    void end_frame() override;
    void upload_imgui_fonts() override;

    [[nodiscard]] Frame& get_current_frame();
    [[nodiscard]] const Frame& get_current_frame() const;
    [[nodiscard]] const Framebuffer& get_current_framebuffer() const;
    [[nodiscard]] static bool is_supported();
};
}
#endif
