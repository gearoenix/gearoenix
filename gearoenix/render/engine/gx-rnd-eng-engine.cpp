#include "gx-rnd-eng-engine.hpp"

#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "../../physics/gx-phs-engine.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/gx-plt-runtime-configuration.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../font/gx-rnd-fnt-manager.hpp"
#include "../gizmo/gx-rnd-gzm-manager.hpp"
#include "../imgui/gx-rnd-imgui-type-table.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"
#include "../light/gx-rnd-lt-manager.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../reflection/gx-rnd-rfl-manager.hpp"
#include "../scene/gx-rnd-scn-manager.hpp"
#include "../skybox/gx-rnd-sky-manager.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"

#include <cinttypes>

#if GX_RENDER_VULKAN_ENABLED
#include "../../vulkan/engine/gx-vk-eng-engine.hpp"
#endif

#if GX_RENDER_DIRECT3D_ENABLED
#include "../../direct3d/gx-d3d-engine.hpp"
#endif

#if GX_RENDER_METAL_ENABLED
#include "../../metal/gx-mtl-engine.hpp"
#endif

#if GX_RENDER_OPENGL_ENABLED
#include "../../opengl/gx-gl-engine.hpp"
#endif

gearoenix::render::engine::Type gearoenix::render::engine::Engine::engine_type;
std::thread::id gearoenix::render::engine::Engine::jobs_thread_id;
std::uint8_t gearoenix::render::engine::Engine::frames_count = static_cast<std::uint8_t>(-1);
std::uint8_t gearoenix::render::engine::Engine::frame_number = 0;
std::uint8_t gearoenix::render::engine::Engine::next_frame_number = static_cast<std::uint8_t>(-1);
std::uint8_t gearoenix::render::engine::Engine::previous_frame_number = static_cast<std::uint8_t>(-1);
std::uint64_t gearoenix::render::engine::Engine::frame_number_from_start = static_cast<std::uint64_t>(-1);
gearoenix::core::fp_t gearoenix::render::engine::Engine::minimum_frame_time = 1.0 / 121.0;
gearoenix::core::fp_t gearoenix::render::engine::Engine::delta_time = 0.0f;
std::chrono::time_point<gearoenix::render::engine::Engine::clock_t> gearoenix::render::engine::Engine::last_frame_time = clock_t::now();
bool gearoenix::render::engine::Engine::half_depth_clip = false;

gearoenix::render::engine::Engine::Engine(const Type in_engine_type)
    : Singleton(this)
    , world(new core::ecs::World())
    , physics_engine(new physics::Engine())
    , font_manager(new font::Manager())
    , gizmo_manager(new gizmo::Manager())
{
    jobs_thread_id = std::this_thread::get_id();
    engine_type = in_engine_type;
    last_frame_time = clock_t::now();
    core::job::register_current_thread();
}

const boost::container::flat_set<gearoenix::render::engine::Type>& gearoenix::render::engine::Engine::get_available_engines()
{
    static const auto available_engines = [] {
        boost::container::flat_set<Type> result;
#if GX_RENDER_DIRECT3D_ENABLED
        result.insert(Type::Direct3D);
#endif

#if GX_RENDER_METAL_ENABLED
        result.insert(Type::Metal);
#endif

#if GX_RENDER_VULKAN_ENABLED
        if (vulkan::engine::Engine::is_supported()) {
            result.insert(Type::Vulkan);
        }
#endif

#if GX_RENDER_OPENGL_ENABLED
        result.insert(Type::OpenGL);
#endif
        return result;
    }();
    return available_engines;
}

std::unique_ptr<gearoenix::render::engine::Engine> gearoenix::render::engine::Engine::construct()
{
    std::unique_ptr<Engine> result;
    const auto& configuration = platform::RuntimeConfiguration::get();

#if GX_RENDER_VULKAN_ENABLED
    if (configuration.get_vulkan_render_backend_enabled() && get_available_engines().contains(Type::Vulkan)) {
        result = std::make_unique<vulkan::engine::Engine>();
    }
#endif

#if GX_RENDER_DIRECT3D_ENABLED
    if (result == nullptr && configuration.get_direct3dx_render_backend_enabled() && get_available_engines().contains(Type::Direct3D)) {
        result = d3d::Engine::construct(platform_application);
    }
#endif

#if GX_RENDER_METAL_ENABLED
    if (result == nullptr && configuration.get_metal_render_backend_enabled() && get_available_engines().contains(Type::Metal)) {
        result = metal::Engine::construct(platform_application);
    }
#endif

#if GX_RENDER_OPENGL_ENABLED
    if (result == nullptr && configuration.get_opengl_render_backend_enabled() && get_available_engines().contains(Type::OpenGL)) {
        result = gl::Engine::construct();
    }
#endif

    GX_CHECK_NOT_EQUAL(result, nullptr);
    return result;
}

gearoenix::render::engine::Engine::~Engine() = default;

void gearoenix::render::engine::Engine::start_frame()
{
    GX_PROFILE_NEW_FRAME;
    if (const auto diff = minimum_frame_time - std::chrono::duration<core::fp_t>(clock_t::now() - last_frame_time).count(); diff > 0.0) {
        std::this_thread::sleep_for(std::chrono::duration<core::fp_t>(diff));
    }
    const auto now = clock_t::now();
    delta_time = std::chrono::duration<core::fp_t>(now - last_frame_time).count();
    GX_ASSERT_D(delta_time > 0.0);
    ImGui::GetIO().DeltaTime = static_cast<float>(delta_time);
    last_frame_time = now;
    previous_frame_number = frame_number_from_start % frames_count;
    ++frame_number_from_start;
    frame_number = frame_number_from_start % frames_count;
    next_frame_number = (frame_number_from_start + 1) % frames_count;
    core::ecs::World::get().update();

    if constexpr (GX_DEBUG_MODE) {
        static core::fp_t time_lapsed = 0.0;
        static core::fp_t counter = 0.0;
        ++counter;
        time_lapsed += delta_time;
        if (time_lapsed >= 5.0) {
            GX_LOG_D("Frame Per Second: " << (counter / time_lapsed));
            time_lapsed = 0.0;
            counter = 0.0;
        }
    }

    GX_PROFILE_EXP(buffer_manager->start_frame());
}

void gearoenix::render::engine::Engine::end_frame()
{
    // Don't mistake the following with the actual start of a frame.
    // In start_frame of Engine, we prepare everything for the interactions of the user of the engine.
    GX_PROFILE_EXP(physics_engine->start_frame());
    GX_PROFILE_EXP(camera_manager->update());
    GX_PROFILE_EXP(light_manager->update());
    GX_PROFILE_EXP(reflection_manager->update());
    GX_PROFILE_EXP(scene_manager->update());
    GX_PROFILE_EXP(physics_engine->end_frame());
    GX_PROFILE_EXP(buffer_manager->end_frame());
}

void gearoenix::render::engine::Engine::window_resized()
{
    camera_manager->window_resized();
}

void gearoenix::render::engine::Engine::show_debug_gui()
{
    imgui::tree_scope(this, [] {
        ImGui::Text("Type: %s", to_string(engine_type));
        ImGui::Text("Frames Count: %" PRIu8, frames_count);
        core::ecs::World::get().show_debug_gui();
        // TODO: I have to show all other things
    });
}

void gearoenix::render::engine::Engine::flush() { }
