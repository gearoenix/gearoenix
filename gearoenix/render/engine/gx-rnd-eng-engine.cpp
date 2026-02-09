#include "gx-rnd-eng-engine.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-engine.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/gx-plt-runtime-configuration.hpp"
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

#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../vulkan/engine/gx-vk-eng-engine.hpp"
#endif

#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../../direct3d/gx-d3d-engine.hpp"
#endif

#ifdef GX_RENDER_METAL_ENABLED
#include "../../metal/gx-mtl-engine.hpp"
#endif

#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../opengl/gx-gl-engine.hpp"
#endif

gearoenix::render::engine::Engine::Engine(const Type engine_type)
    : Singleton(this)
    , world(new core::ecs::World())
    , jobs_thread_id(std::this_thread::get_id())
    , engine_type(engine_type)
    , physics_engine(new physics::Engine())
    , font_manager(new font::Manager())
    , gizmo_manager(new gizmo::Manager())
    , last_frame_time(clock_t::now())
{
    core::job::register_current_thread();
}

std::set<gearoenix::render::engine::Type> gearoenix::render::engine::Engine::get_available_engines()
{
    std::set<Type> result;
#ifdef GX_RENDER_DIRECT3D_ENABLED
    result.insert(Type::Direct3D);
#endif
#ifdef GX_RENDER_METAL_ENABLED
    result.insert(Type::Metal);
#endif
#ifdef GX_RENDER_VULKAN_ENABLED
    if (vulkan::engine::Engine::is_supported()) {
        result.insert(Type::Vulkan);
    }
#endif
#ifdef GX_RENDER_OPENGL_ENABLED
    result.insert(Type::OpenGL);
#endif
    return result;
}

std::unique_ptr<gearoenix::render::engine::Engine> gearoenix::render::engine::Engine::construct()
{
    std::unique_ptr<Engine> result;
    const auto& configuration = platform::RuntimeConfiguration::get();
#ifdef GX_RENDER_VULKAN_ENABLED
    if (configuration.get_vulkan_render_backend_enabled() && vulkan::engine::Engine::is_supported()) {
        result = std::make_unique<vulkan::engine::Engine>();
    }
#endif
#ifdef GX_RENDER_DIRECT3D_ENABLED
    if (result == nullptr && configuration.get_direct3dx_render_backend_enabled() && d3d::Engine::is_supported()) {
        result = d3d::Engine::construct(platform_application);
    }
#endif
#ifdef GX_RENDER_METAL_ENABLED
    if (result == nullptr && configuration.get_metal_render_backend_enabled() && metal::Engine::is_supported()) {
        result = metal::Engine::construct(platform_application);
    }
#endif
#ifdef GX_RENDER_OPENGL_ENABLED
    if (result == nullptr && configuration.get_opengl_render_backend_enabled() && gl::Engine::is_supported()) {
        result = gl::Engine::construct();
    }
#endif
    GX_CHECK_NOT_EQUAL(result, nullptr);
    return result;
}

gearoenix::render::engine::Engine::~Engine() = default;

void gearoenix::render::engine::Engine::start_frame()
{
    if (const auto diff = minimum_frame_time - std::chrono::duration<double>(clock_t::now() - last_frame_time).count(); diff > 0.0) {
        std::this_thread::sleep_for(std::chrono::duration<double>(diff));
    }
    const auto now = clock_t::now();
    delta_time = std::chrono::duration<double>(now - last_frame_time).count();
    GX_ASSERT_D(delta_time > 0.0);
    ImGui::GetIO().DeltaTime = static_cast<float>(delta_time);
    last_frame_time = now;
    previous_frame_number = frame_number_from_start % frames_count;
    ++frame_number_from_start;
    frame_number = frame_number_from_start % frames_count;
    next_frame_number = (frame_number_from_start + 1) % frames_count;
    core::ecs::World::get().update();

    if constexpr (GX_DEBUG_MODE) {
        static double time_lapsed = 0.0;
        static double counter = 0.0;
        ++counter;
        time_lapsed += delta_time;
        if (time_lapsed >= 5.0) {
            GX_LOG_D("Frame Per Second: " << (counter / time_lapsed));
            time_lapsed = 0.0;
            counter = 0.0;
        }
    }
}

void gearoenix::render::engine::Engine::end_frame()
{
    // Don't mistake the following with the actual start of a frame.
    // In start_frame of Engine, we prepare everything for the interactions of the user of the engine.
    physics_engine->start_frame();

    model_manager->update();
    camera_manager->update();
    light_manager->update();
    reflection_manager->update();
    scene_manager->update();
    physics_engine->end_frame();
}

void gearoenix::render::engine::Engine::window_resized()
{
    camera_manager->window_resized();
}

void gearoenix::render::engine::Engine::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        ImGui::Text("Type: %s", to_string(engine_type));
        ImGui::Text("Frames Count: %" PRIu64, frames_count);
        core::ecs::World::get().show_debug_gui();
        // TODO: I have to show all other things
    });
}

void gearoenix::render::engine::Engine::flush()
{
}
