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
    buffer::Manager* vk_buffer_manager = nullptr;
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
    void submit();

public:
    Engine();
    ~Engine() override;
    void start_frame() override;
    void end_frame() override;
    void upload_imgui_fonts() override;
    [[nodiscard]] Frame& get_current_frame();
    [[nodiscard]] const Frame& get_current_frame() const;
    [[nodiscard]] static bool is_supported();
    void flush() override;
    void window_resized() override;
    void hdr_state_changed() override;
};
}
#endif
