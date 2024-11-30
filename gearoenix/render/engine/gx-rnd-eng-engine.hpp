#ifndef GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#include "../gx-rnd-runtime-configuration.hpp"
#include "gx-rnd-eng-specification.hpp"
#include "gx-rnd-eng-type.hpp"
#include <chrono>
#include <memory>
#include <set>
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
struct Engine {
    GX_GET_CVAL_PRT(std::thread::id, jobs_thread_id);
    GX_GET_CVAL_PRT(Type, engine_type);
    GX_GET_RRF_PRT(platform::Application, platform_application);
    GX_GET_UPTR_PRT(physics::Engine, physics_engine);
    GX_GET_CREF_PRT(Specification, specification);
    GX_GET_VAL_PRT(std::uint64_t, frames_count, 2);
    GX_GET_VAL_PRT(std::uint64_t, frame_number, 0);
    GX_GET_VAL_PRT(std::uint64_t, next_frame_number, 1);
    GX_GET_VAL_PRT(std::uint64_t, previous_frame_number, 1);
    GX_GET_VAL_PRT(std::uint64_t, frame_number_from_start, static_cast<decltype(frame_number)>(-1));
    GX_GET_VAL_PRT(double, delta_time, 0.0f);
    GX_GET_UPTR_PRT(scene::Manager, scene_manager);
    GX_GET_UPTR_PRT(mesh::Manager, mesh_manager);
    GX_GET_UPTR_PRT(model::Manager, model_manager);
    GX_GET_UPTR_PRT(material::Manager, material_manager);
    GX_GET_UPTR_PRT(camera::Manager, camera_manager);
    GX_GET_UPTR_PRT(texture::Manager, texture_manager);
    GX_GET_UPTR_PRT(skybox::Manager, skybox_manager);
    GX_GET_UPTR_PRT(reflection::Manager, reflection_manager);
    GX_GET_UPTR_PRT(font::Manager, font_manager);
    GX_GET_UPTR_PRT(light::Manager, light_manager);
    GX_GET_UPTR_PRT(gizmo::Manager, gizmo_manager);
    GX_GET_UPTR_PRT(core::ecs::World, world);
    GX_GET_CREF_PRT(std::chrono::time_point<std::chrono::high_resolution_clock>, last_frame_time);

    Engine(Type engine_type, platform::Application& platform_application);

public:
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    [[nodiscard]] static std::set<Type> get_available_engines();
    [[nodiscard]] static std::unique_ptr<Engine> construct(platform::Application& platform_application);
    virtual ~Engine();
    virtual void start_frame();
    virtual void end_frame();
    virtual void window_resized();
    virtual void upload_imgui_fonts() = 0;
    virtual void show_debug_gui();
};
}
#endif
