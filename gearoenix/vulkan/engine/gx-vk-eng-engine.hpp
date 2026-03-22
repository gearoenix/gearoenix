#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-vk-build-configuration.hpp"

#include <array>
#include <memory>

namespace gearoenix::vulkan {
struct ImGuiManager;
struct Instance;
struct Surface;
struct Swapchain;
}

namespace gearoenix::vulkan::buffer {
struct Manager;
}

namespace gearoenix::vulkan::camera {
struct Manager;
}

namespace gearoenix::vulkan::command {
struct Manager;
}

namespace gearoenix::vulkan::descriptor {
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

namespace gearoenix::vulkan::light {
struct Manager;
}

namespace gearoenix::vulkan::material {
struct Manager;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::mesh {
struct Manager;
}

namespace gearoenix::vulkan::model {
struct Manager;
}

namespace gearoenix::vulkan::pipeline {
struct Manager;
}

namespace gearoenix::vulkan::queue {
struct Graph;
struct Queue;
}

namespace gearoenix::vulkan::reflection {
struct Manager;
}

namespace gearoenix::vulkan::sampler {
struct Manager;
}

namespace gearoenix::vulkan::scene {
struct Manager;
}

namespace gearoenix::vulkan::skybox {
struct Manager;
}

namespace gearoenix::vulkan::texture {
struct Manager;
}

namespace gearoenix::vulkan::engine {
struct Frame;
struct Engine final : render::engine::Engine, core::Singleton<Engine> {
    using frames_t = std::array<std::unique_ptr<Frame>, frames_in_flight>;

private:
    std::unique_ptr<Instance> instance;
    std::unique_ptr<Surface> surface;
    std::unique_ptr<device::Physical> physical_device;
    std::unique_ptr<device::Logical> logical_device;
    std::unique_ptr<Swapchain> swapchain;
    std::unique_ptr<memory::Manager> memory_manager;
    std::unique_ptr<command::Manager> command_manager;
    std::unique_ptr<descriptor::Bindless> bindless_descriptor_manager;
    std::unique_ptr<pipeline::Manager> pipeline_manager;
    std::unique_ptr<buffer::Manager> buffer_manager;
    std::unique_ptr<ImGuiManager> imgui_manager;
    std::unique_ptr<image::Manager> image_manager;
    std::unique_ptr<sampler::Manager> sampler_manager;
    texture::Manager* vk_texture_manager = nullptr;
    mesh::Manager* vk_mesh_manager = nullptr;
    material::Manager* vk_material_manager = nullptr;
    model::Manager* vk_model_manager = nullptr;
    light::Manager* vk_light_manager = nullptr;
    camera::Manager* vk_camera_manager = nullptr;
    scene::Manager* vk_scene_manager = nullptr;
    skybox::Manager* vk_skybox_manager = nullptr;
    reflection::Manager* vk_reflection_manager = nullptr;
    frames_t frames;
    std::unique_ptr<queue::Queue> render_queue;

    void initialize_frame();
    void window_resized() override;

public:
    [[nodiscard]] const std::unique_ptr<Instance>& get_instance() const { return instance; }
    [[nodiscard]] const std::unique_ptr<Surface>& get_surface() const { return surface; }
    [[nodiscard]] const std::unique_ptr<device::Physical>& get_physical_device() const { return physical_device; }
    [[nodiscard]] const std::unique_ptr<device::Logical>& get_logical_device() const { return logical_device; }
    [[nodiscard]] const std::unique_ptr<Swapchain>& get_swapchain() const { return swapchain; }
    [[nodiscard]] const std::unique_ptr<memory::Manager>& get_memory_manager() const { return memory_manager; }
    [[nodiscard]] const std::unique_ptr<command::Manager>& get_command_manager() const { return command_manager; }
    [[nodiscard]] const std::unique_ptr<descriptor::Bindless>& get_bindless_descriptor_manager() const { return bindless_descriptor_manager; }
    [[nodiscard]] const std::unique_ptr<pipeline::Manager>& get_pipeline_manager() const { return pipeline_manager; }
    [[nodiscard]] const std::unique_ptr<buffer::Manager>& get_buffer_manager() const { return buffer_manager; }
    [[nodiscard]] const std::unique_ptr<ImGuiManager>& get_imgui_manager() const { return imgui_manager; }
    [[nodiscard]] const std::unique_ptr<image::Manager>& get_image_manager() const { return image_manager; }
    [[nodiscard]] const std::unique_ptr<sampler::Manager>& get_sampler_manager() const { return sampler_manager; }
    [[nodiscard]] const texture::Manager* get_vk_texture_manager() const { return vk_texture_manager; }
    [[nodiscard]] const mesh::Manager* get_vk_mesh_manager() const { return vk_mesh_manager; }
    [[nodiscard]] const material::Manager* get_vk_material_manager() const { return vk_material_manager; }
    [[nodiscard]] const model::Manager* get_vk_model_manager() const { return vk_model_manager; }
    [[nodiscard]] const light::Manager* get_vk_light_manager() const { return vk_light_manager; }
    [[nodiscard]] const camera::Manager* get_vk_camera_manager() const { return vk_camera_manager; }
    [[nodiscard]] const scene::Manager* get_vk_scene_manager() const { return vk_scene_manager; }
    [[nodiscard]] const skybox::Manager* get_vk_skybox_manager() const { return vk_skybox_manager; }
    [[nodiscard]] const reflection::Manager* get_vk_reflection_manager() const { return vk_reflection_manager; }
    [[nodiscard]] const frames_t& get_frames() const { return frames; }
    [[nodiscard]] const std::unique_ptr<queue::Queue>& get_render_queue() const { return render_queue; }

    Engine();
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    ~Engine() override;
    void start_frame() override;
    void end_frame() override;
    void upload_imgui_fonts() override;
    void submit();
    [[nodiscard]] Frame& get_current_frame();
    [[nodiscard]] const Frame& get_current_frame() const;
    [[nodiscard]] static bool is_supported();
    void flush() override;
    void upload_uniforms();
};
}
#endif
