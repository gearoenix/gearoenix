#ifndef GEAROENIX_VULKAN_ENGINE_HPP
#define GEAROENIX_VULKAN_ENGINE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../descriptor/gx-vk-des-manager.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-render-pass.hpp"
#include "../gx-vk-surface.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include <functional>
#include <mutex>

namespace gearoenix::vulkan {
struct ImGuiManager;
}

namespace gearoenix::vulkan::mesh {
struct Manager;
}

namespace gearoenix::vulkan::queue {
struct Queue;
}

namespace gearoenix::vulkan::engine {
struct Frame;
struct Engine final : public render::engine::Engine {
    GX_GET_CREF_PRV(Instance, instance)
    GX_GET_CREF_PRV(Surface, surface)
    GX_GET_CREF_PRV(device::Physical, physical_device)
    GX_GET_REF_PRV(device::Logical, logical_device)
    GX_GET_CREF_PRV(Swapchain, swapchain)
    GX_GET_CREF_PRV(memory::Manager, memory_manager)
    GX_GET_REF_PRV(command::Manager, command_manager)
    GX_GET_CREF_PRV(descriptor::Manager, descriptor_manager)
    GX_GET_CREF_PRV(pipeline::Manager, pipeline_manager)
    GX_GET_REF_PRV(buffer::Manager, buffer_manager)
    GX_GET_CREF_PRV(image::View, depth_stencil)
    GX_GET_CREF_PRV(RenderPass, render_pass)
    GX_GET_UPTR_PRV(queue::Queue, graphic_queue)
    GX_GET_UPTR_PRV(ImGuiManager, imgui_manager)
    GX_GET_CREF_PRV(std::shared_ptr<mesh::Manager>, mesh_manager)
    GX_GET_CREF_PRV(std::vector<std::unique_ptr<Frame>>, frames)
    GX_GET_VAL_PRV(std::uint32_t, swapchain_image_index, 0)
    GX_GET_VAL_PRV(bool, swapchain_image_is_valid, true)
    //    GX_GET_CREF_PRV(std::shared_ptr<sampler::Manager>, sampler_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<image::Manager>, image_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<texture::MainTarget>, vulkan_main_render_target)
    //    GX_GET_CREF_PRV(std::vector<std::shared_ptr<command::Buffer>>, upload_command_buffers)
    //    GX_GET_CREF_PRV(std::vector<std::shared_ptr<sync::Semaphore>>, upload_semaphore)
private:
    void initialize_frame() noexcept;

public:
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    explicit Engine(const platform::Application& platform_application) noexcept;
    ~Engine() noexcept final;
    void start_frame() noexcept final;
    void end_frame() noexcept final;
    void upload_imgui_fonts() noexcept final;
    void create_mesh(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept final;
    [[nodiscard]] Frame& get_current_frame() noexcept;
    [[nodiscard]] const Frame& get_current_frame() const noexcept;
    [[nodiscard]] const Framebuffer& get_current_framebuffer() const noexcept;
    [[nodiscard]] static bool is_supported() noexcept;
};
}
#endif
#endif
