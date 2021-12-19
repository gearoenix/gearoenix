#include "gx-plt-application.hpp"
#include "../core/gx-cr-application.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include <imgui.h>

gearoenix::platform::BaseApplication::BaseApplication(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& configuration) noexcept
    : configuration(configuration)
    , arguments(GX_MAIN_ENTRY_ARGS)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    auto io_imgui = ImGui::GetIO();
    io_imgui.Fonts->AddFontDefault();
    io_imgui.BackendPlatformName = configuration.get_application_name().c_str();
    key::initialize_imgui_keymap();

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
    window_x = x;
    window_y = y;
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
    const auto dw = static_cast<double>(w);
    const auto dh = static_cast<double>(h);
    window_aspect_ratio = dw / dh;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<decltype(io.DisplaySize.x)>(window_width);
    io.DisplaySize.y = static_cast<decltype(io.DisplaySize.y)>(window_height);
    last_time_window_resized = std::chrono::high_resolution_clock::now();
}

void gearoenix::platform::BaseApplication::update_window() noexcept
{
    if (window_resizing && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - last_time_window_resized).count() > configuration.get_window_resizing_event_interval_ms()) {
        // TODO fire the resizing event
        window_resizing = false;
    }
}

void gearoenix::platform::BaseApplication::initialize_mouse_position(const double x, const double y) noexcept
{
    pre_mouse_x = mouse_x = x;
    pre_mouse_y = mouse_y = y;
    pre_mouse_x_nrm = mouse_x_nrm = (x - static_cast<double>(window_width) * 0.5) / static_cast<double>(window_height);
    pre_mouse_y_nrm = mouse_y_nrm = y / static_cast<double>(window_height) - 0.5;
    ImGui::GetIO().MousePos = { static_cast<float>(x), static_cast<float>(y) };
}

void gearoenix::platform::BaseApplication::update_mouse_position(double x, double y) noexcept
{
    pre_mouse_x = mouse_x;
    pre_mouse_y = mouse_y;
    mouse_x = x;
    mouse_y = y;
    pre_mouse_x_nrm = mouse_x_nrm;
    pre_mouse_y_nrm = mouse_y_nrm;
    mouse_x_nrm = (x - static_cast<double>(window_width) * 0.5) / static_cast<double>(window_height);
    mouse_y_nrm = y / static_cast<double>(window_height) - 0.5;
    delta_mouse_x = mouse_x - pre_mouse_x;
    delta_mouse_y = mouse_y - pre_mouse_y;
    delta_mouse_x_nrm = mouse_x_nrm - pre_mouse_x_nrm;
    delta_mouse_y_nrm = mouse_y_nrm - pre_mouse_y_nrm;
    ImGui::GetIO().MousePos = { static_cast<float>(x), static_cast<float>(y) };
}

void gearoenix::platform::BaseApplication::mouse_key(const key::Id k, const key::Action a) noexcept
{
    ImGui::GetIO().MouseDown[key::convert_mouse_to_imgui(k)] = a == key::Action::Press;
}

void gearoenix::platform::BaseApplication::mouse_wheel(const double v) noexcept
{
    ImGui::GetIO().MouseWheel += static_cast<float>(v);
}

void gearoenix::platform::BaseApplication::keyboard_key(const key::Id k, const key::Action a) noexcept
{
    const auto pressed = a == key::Action::Press;
    auto& io = ImGui::GetIO();
    io.KeysDown[static_cast<int>(k)] = pressed;
    switch (k) {
    case key::Id::LeftControl:
    case key::Id::RightControl:
        io.KeyCtrl = pressed;
        break;
    case key::Id::LeftShift:
    case key::Id::RightShift:
        io.KeyShift = pressed;
        break;
    case key::Id::LeftAlt:
    case key::Id::RightAlt:
        io.KeyAlt = pressed;
        break;
    case key::Id::LeftSuper:
    case key::Id::RightSuper:
        io.KeySuper = pressed;
        break;
    default:
        break;
    }
}

void gearoenix::platform::BaseApplication::character_input(const char16_t ch) noexcept
{
    ImGui::GetIO().AddInputCharacterUTF16(ch);
}

void gearoenix::platform::BaseApplication::initialize_engine(Application& app) noexcept
{
    render_engine = render::engine::Engine::construct(app);
}

void gearoenix::platform::BaseApplication::going_to_be_closed() noexcept
{
    running = false;
    // TODO
}

void gearoenix::platform::BaseApplication::terminate() noexcept
{
    core_application = nullptr;
    render_engine = nullptr;
}

void gearoenix::platform::BaseApplication::update() noexcept
{
    update_window();
    render_engine->start_frame();
    core_application->update();
    render_engine->end_frame();
}

void gearoenix::platform::BaseApplication::initialize_core_application(
    Application& app,
    core::Application* const core_app) noexcept
{
    if (nullptr == core_app) {
        core_application = std::make_unique<core::Application>(app);
    } else {
        core_application = std::unique_ptr<core::Application>(core_app);
    }
}
