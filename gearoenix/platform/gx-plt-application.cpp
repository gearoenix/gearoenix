#include "gx-plt-application.hpp"
#include "../core/gx-cr-application.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-plt-log.hpp"
#include <imgui.h>

gearoenix::platform::BaseApplication::BaseApplication(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& configuration) noexcept
    : configuration(configuration)
    , arguments(GX_MAIN_ENTRY_ARGS)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().Fonts->AddFontDefault();

    if (!configuration.get_fullscreen()) {
        initialize_window_size(
            static_cast<decltype(window_width)>(configuration.get_window_width()),
            static_cast<decltype(window_height)>(configuration.get_window_height()));
    }
}

gearoenix::platform::BaseApplication::~BaseApplication() noexcept = default;

void gearoenix::platform::BaseApplication::initialize_window_position(
    const int x, const int y) noexcept
{
    pre_window_x = window_x = x;
    pre_window_y = window_y = y;
}

void gearoenix::platform::BaseApplication::initialize_window_size(const int w, const int h) noexcept
{
    update_window_size(w, h);
    window_resizing = false;
}

void gearoenix::platform::BaseApplication::update_window_size(const int w, const int h) noexcept
{
    window_resizing = true;
    window_width = w;
    window_height = h;
    window_aspect_ratio = static_cast<decltype(window_aspect_ratio)>(w) / static_cast<decltype(window_aspect_ratio)>(h);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<decltype(io.DisplaySize.x)>(window_width);
    io.DisplaySize.y = static_cast<decltype(io.DisplaySize.y)>(window_height);
    last_time_window_resized = std::chrono::high_resolution_clock::now();
}

void gearoenix::platform::BaseApplication::initialize_mouse_position(const double x, const double y) noexcept
{
    pre_mouse_x = mouse_x = x;
    pre_mouse_y = mouse_y = y;
    // TODO
}

void gearoenix::platform::BaseApplication::going_to_be_closed() noexcept
{
    running = false;
    // TODO
}

void gearoenix::platform::BaseApplication::update() noexcept
{
    if (window_resizing && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - last_time_window_resized).count() > configuration.get_window_resizing_event_interval_ms()) {
        // TODO fire the resizing event
        window_resizing = false;
    }

    render_engine->start_frame();
    core_application->update();
    render_engine->end_frame();
}
