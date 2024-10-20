#include "gx-rnd-eng-engine.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-string.hpp"
#include "../../physics/gx-phs-engine.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../font/gx-rnd-fnt-manager.hpp"
#include "../light/gx-rnd-lt-manager.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../reflection/gx-rnd-rfl-manager.hpp"
#include "../scene/gx-rnd-scn-manager.hpp"
#include "../skybox/gx-rnd-sky-manager.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"

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

#include <imgui/imgui.h>

gearoenix::render::engine::Engine::Engine(
    const Type engine_type,
    platform::Application& platform_application)
    : jobs_thread_id(std::this_thread::get_id())
    , engine_type(engine_type)
    , platform_application(platform_application)
    , physics_engine(new physics::Engine(*this))
    , scene_manager(new scene::Manager(*this))
    , font_manager(new font::Manager(*this))
    , world(new core::ecs::World())
    , last_frame_time(std::chrono::high_resolution_clock::now())
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

std::unique_ptr<gearoenix::render::engine::Engine> gearoenix::render::engine::Engine::construct(
    platform::Application& platform_application)
{
    std::unique_ptr<Engine> result;
    const auto& configuration = platform::RuntimeConfiguration::get(&platform_application);
#ifdef GX_RENDER_VULKAN_ENABLED
    if (configuration.get_vulkan_render_backend_enabled() && vulkan::engine::Engine::is_supported()) {
        result = std::make_unique<vulkan::engine::Engine>(platform_application);
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
        result = gl::Engine::construct(platform_application);
    }
#endif
    GX_CHECK_NOT_EQUAL(result, nullptr);
    return result;
}

gearoenix::render::engine::Engine::~Engine() = default;

void gearoenix::render::engine::Engine::start_frame()
{
    const std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> delta_time_duration = now - last_frame_time;
    delta_time = delta_time_duration.count();
    ImGui::GetIO().DeltaTime = static_cast<float>(delta_time);
    last_frame_time = now;
    previous_frame_number = frame_number_from_start % frames_count;
    ++frame_number_from_start;
    frame_number = frame_number_from_start % frames_count;
    next_frame_number = (frame_number_from_start + 1) % frames_count;
    world->update();
}

void gearoenix::render::engine::Engine::end_frame()
{
    physics_engine->start_frame(); // Don't mistake this with the actual start of frame, in start_frame of Engine, we prepare every thing for user interaction
    camera_manager->update();
    scene_manager->update();
    reflection_manager->update();
    physics_engine->end_frame();
}

void gearoenix::render::engine::Engine::window_resized()
{
    camera_manager->window_resized();
}

void gearoenix::render::engine::Engine::show_debug_gui()
{
    if (ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        ImGui::Text("Type: %s", to_string(engine_type));
        ImGui::Text("Frames Count: %zu", frames_count);
        world->show_debug_gui();
        // TODO: I have to show all other things
        ImGui::TreePop();
    }
}
