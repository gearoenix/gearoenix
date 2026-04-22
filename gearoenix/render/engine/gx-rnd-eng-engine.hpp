#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-rnd-runtime-configuration.hpp"
#include "gx-rnd-eng-specification.hpp"
#include "gx-rnd-eng-type.hpp"

#include <boost/container/flat_set.hpp>

#include <chrono>
#include <memory>
#include <thread>

namespace gearoenix::core::ecs {
struct World;
}

namespace gearoenix::physics {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::render::buffer {
struct Manager;
}

namespace gearoenix::render::camera {
struct Manager;
}

namespace gearoenix::render::font {
struct Manager;
}

namespace gearoenix::render::gizmo {
struct Manager;
}

namespace gearoenix::render::light {
struct Manager;
}

namespace gearoenix::render::material {
struct Manager;
}

namespace gearoenix::render::mesh {
struct Manager;
}

namespace gearoenix::render::model {
struct Manager;
}

namespace gearoenix::render::reflection {
struct Manager;
}

namespace gearoenix::render::scene {
struct Manager;
}

namespace gearoenix::render::skybox {
struct Manager;
}

namespace gearoenix::render::texture {
struct Manager;
}

namespace gearoenix::render::engine {
struct Engine : core::Singleton<Engine> {
    using clock_t = std::chrono::steady_clock;

    GX_GET_CREF_PRT(Specification, specification);

    static Type engine_type;
    static std::thread::id jobs_thread_id;
    static std::uint8_t frames_count; // = static_cast<std::uint64_t>(-1)
    static std::uint8_t frame_number; // = 0
    static std::uint8_t next_frame_number; // = 1
    static std::uint8_t previous_frame_number; // = 1
    static std::uint64_t frame_number_from_start; // = static_cast<std::uint64_t>(-1)
    static core::fp_t minimum_frame_time; // = 1.0 / 121.0, limits the FPS
    static core::fp_t delta_time; // = 0.0f
    static std::chrono::time_point<clock_t> last_frame_time;
    static bool half_depth_clip; // = false

    std::unique_ptr<core::ecs::World> world;
    std::unique_ptr<physics::Engine> physics_engine;
    std::unique_ptr<scene::Manager> scene_manager;
    std::unique_ptr<mesh::Manager> mesh_manager;
    std::unique_ptr<model::Manager> model_manager;
    std::unique_ptr<material::Manager> material_manager;
    std::unique_ptr<camera::Manager> camera_manager;
    std::unique_ptr<texture::Manager> texture_manager;
    std::unique_ptr<skybox::Manager> skybox_manager;
    std::unique_ptr<reflection::Manager> reflection_manager;
    std::unique_ptr<font::Manager> font_manager;
    std::unique_ptr<light::Manager> light_manager;
    std::unique_ptr<gizmo::Manager> gizmo_manager;
    std::unique_ptr<buffer::Manager> buffer_manager;

    explicit Engine(Type in_engine_type);

public:
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;
    ~Engine() override;

    [[nodiscard]] static const boost::container::flat_set<Type>& get_available_engines();
    [[nodiscard]] static std::unique_ptr<Engine> construct();
    [[nodiscard]] static Type get_engine_type() { return engine_type; }
    [[nodiscard]] static std::thread::id get_jobs_thread_id() { return jobs_thread_id; }
    [[nodiscard]] static std::uint64_t get_frames_count() { return frames_count; }
    [[nodiscard]] static std::uint64_t get_frame_number() { return frame_number; }
    [[nodiscard]] static std::uint64_t get_frame_number_from_start() { return frame_number_from_start; }
    [[nodiscard]] static core::fp_t get_delta_time() { return delta_time; }
    [[nodiscard]] static bool get_half_depth_clip() { return half_depth_clip; }

    virtual void start_frame();
    virtual void end_frame();
    virtual void window_resized();
    virtual void upload_imgui_fonts() = 0;
    virtual void show_debug_gui();
    virtual void flush();
};
}