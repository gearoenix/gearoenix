#include "gx-plt-application.hpp"
#include "../core/event/gx-cr-ev-engine.hpp"
#include "../core/gx-cr-application.hpp"
#include "../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include <imgui.h>

constexpr double click_time_threshold = 0.3;
constexpr double click_distance_threshold = 0.1;

gearoenix::platform::BaseApplication::BaseApplication(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& configuration) noexcept
    : configuration(configuration)
    , arguments(GX_MAIN_ENTRY_ARGS)
    , event_engine(new core::event::Engine())
{
    core::sync::ParallelFor::initialise();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    auto io_imgui = ImGui::GetIO();
    io_imgui.Fonts->AddFontDefault();
    io_imgui.BackendPlatformName = configuration.get_application_name().c_str();
    key::initialize_imgui_keymap();

    if (!configuration.get_fullscreen()) {
        initialize_window_size(
            static_cast<int>(configuration.get_window_width()),
            static_cast<int>(configuration.get_window_height()));
    }
}

gearoenix::platform::BaseApplication::~BaseApplication() noexcept = default;

void gearoenix::platform::BaseApplication::initialize_window_position(const int x, const int y) noexcept
{
    window_size.x = x;
    window_size.y = y;
}

void gearoenix::platform::BaseApplication::initialize_window_size(const int w, const int h) noexcept
{
    update_window_size(w, h);
    previous_window_size.x = w;
    previous_window_size.y = h;
    window_resizing = false;
}

void gearoenix::platform::BaseApplication::update_window_size(const int w, const int h) noexcept
{
    window_resizing = true;
    window_size.x = w;
    window_size.y = h;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<decltype(io.DisplaySize.x)>(window_size.x);
    io.DisplaySize.y = static_cast<decltype(io.DisplaySize.y)>(window_size.y);
    last_time_window_resized = std::chrono::high_resolution_clock::now();
}

void gearoenix::platform::BaseApplication::update_window() noexcept
{
    if (window_resizing && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - last_time_window_resized).count() > configuration.get_window_resizing_event_interval_ms()) {
        window_resizing = false;
        render_engine->window_resized();
        event_engine->broadcast(core::event::Data(core::event::Id::PlatformWindowSizeChange,
            core::event::platform::WindowSizeChangeData(
                previous_window_size.x,
                previous_window_size.y,
                window_size.x,
                window_size.y)));
        previous_window_size = window_size;
    }
}

void gearoenix::platform::BaseApplication::initialize_mouse_position(const double x, const double y) noexcept
{
    mouse_previous_position = mouse_position = { x, y };
    mouse_previous_normalised_position = mouse_normalised_position = normalise_position(x, y);
    ImGui::GetIO().MousePos = { static_cast<float>(x), static_cast<float>(y) };
}

void gearoenix::platform::BaseApplication::update_mouse_position(const double x, const double y) noexcept
{
    mouse_previous_position = mouse_position;
    mouse_previous_normalised_position = mouse_normalised_position;

    mouse_position = { x, y };
    mouse_normalised_position = normalise_position(x, y);

    ImGui::GetIO().MousePos = { static_cast<float>(x), static_cast<float>(y) };

    event_engine->broadcast(
        core::event::Data(
            core::event::Id::MovementMouse,
            core::event::movement::Mouse(
                mouse_position,
                mouse_normalised_position,
                mouse_previous_position,
                mouse_previous_normalised_position)));
}

void gearoenix::platform::BaseApplication::mouse_key(const key::Id k, const key::Action a) noexcept
{
    ImGui::GetIO().MouseDown[key::convert_mouse_to_imgui(k)] = a == key::Action::Press;
    event_engine->broadcast(
        core::event::Data(
            core::event::Id::ButtonMouse,
            core::event::button::Mouse(
                a, k, mouse_normalised_position, mouse_position)));
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
    event_engine->broadcast(
        core::event::Data(
            core::event::Id::ButtonKeyboard,
            core::event::button::Keyboard(a, k)));
}

void gearoenix::platform::BaseApplication::character_input(const char16_t ch) noexcept
{
    ImGui::GetIO().AddInputCharacterUTF16(ch);
}

void gearoenix::platform::BaseApplication::touch_down(const FingerId finger_id, const double x, const double y) noexcept
{
    const core::event::Point2D p({ x, y }, normalise_position(x, y));
    touch_states.emplace(finger_id, p);
    event_engine->broadcast(
        core::event::Data(
            core::event::Id::Touch,
            core::event::touch::Data(
                p,
                finger_id,
                core::event::touch::Action::Down)));
}

void gearoenix::platform::BaseApplication::touch_move(const FingerId finger_id, const double x, const double y) noexcept
{
    const auto search = touch_states.find(finger_id);
    if (touch_states.end() == search)
        return;
    auto& p = search->second;
    p.update({ x, y }, normalise_position(x, y));
    if (touch_states.size() == 1 && p.get_delta_start_time() > click_time_threshold) {
        const core::event::gesture::Drag2D d(p);
        event_engine->broadcast(core::event::Data(core::event::Id::GestureDrag2D, d));
        event_engine->broadcast(core::event::Data(
            core::event::Id::GestureTouchDrag,
            core::event::gesture::TouchDrag(d, finger_id)));
    } else if (touch_states.size() == 2) {
        const auto first = touch_states.begin();
        const auto second = first + 1;
        const core::event::gesture::Scale s(first->second, second->second);
        event_engine->broadcast(core::event::Data(core::event::Id::GestureScale, s));
        event_engine->broadcast(core::event::Data(
            core::event::Id::GestureTouchScale,
            core::event::gesture::TouchScale(s, first->first, second->first)));
    }
    event_engine->broadcast(
        core::event::Data(
            core::event::Id::Touch,
            core::event::touch::Data(
                p,
                finger_id,
                core::event::touch::Action::Move)));
}

void gearoenix::platform::BaseApplication::touch_up(const FingerId finger_id) noexcept
{
    const auto search = touch_states.find(finger_id);
    if (touch_states.end() == search)
        return;
    const auto& p = search->second;
    if (touch_states.size() == 1 && search->second.get_delta_start_position().length() < click_distance_threshold && p.get_delta_start_time() < click_time_threshold) {
        core::event::gesture::Click click(p);
        event_engine->broadcast(
            core::event::Data(
                core::event::Id::GestureClick,
                click));
        event_engine->broadcast(
            core::event::Data(
                core::event::Id::GestureTouchClick,
                core::event::gesture::TouchClick(
                    click,
                    finger_id)));
    }
    event_engine->broadcast(
        core::event::Data(
            core::event::Id::Touch,
            core::event::touch::Data(
                p,
                finger_id,
                core::event::touch::Action::Up)));
    touch_states.erase(search);
}

void gearoenix::platform::BaseApplication::touch_cancel(const FingerId finger_id) noexcept
{
    const auto search = touch_states.find(finger_id);
    if (touch_states.end() == search)
        return;
    event_engine->broadcast(
        core::event::Data(
            core::event::Id::Touch,
            core::event::touch::Data(
                search->second,
                finger_id,
                core::event::touch::Action::Cancel)));
    touch_states.erase(finger_id);
}

void gearoenix::platform::BaseApplication::initialize_engine(Application& app) noexcept
{
    render_engine = render::engine::Engine::construct(app);
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

void gearoenix::platform::BaseApplication::going_to_be_closed() noexcept
{
    running = false;
    GX_TODO;
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

double gearoenix::platform::BaseApplication::normalise_x(const double x) const noexcept
{
    return (x - static_cast<double>(window_size.x) * 0.5) / static_cast<double>(window_size.y);
}

double gearoenix::platform::BaseApplication::normalise_y(const double y) const noexcept
{
    return y / static_cast<double>(window_size.y) - 0.5;
}

gearoenix::math::Vec2<double> gearoenix::platform::BaseApplication::normalise_position(const double x, const double y) const noexcept
{
    return math::Vec2<double>(normalise_x(x), normalise_y(y));
}
