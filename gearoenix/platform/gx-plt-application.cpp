#include "gx-plt-application.hpp"
#include "../audio/gx-au-engine.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/event/gx-cr-ev-engine.hpp"
#include "../core/gx-cr-application.hpp"
#include "../core/sync/gx-cr-sync-thread.hpp"
#include "../net/gx-net-manager.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "../render/imgui/gx-rnd-imgui-popup.hpp"
#include "gx-plt-file-chooser.hpp"
#include "gx-plt-runtime-configuration.hpp"
#include "stream/gx-plt-stm-stream.hpp"

#include <ImGui/imgui.h>

namespace {
constexpr double click_time_threshold = 0.3;
constexpr double click_distance_threshold = 0.1;

void initialise_default_font()
{
    const auto [content, read_bytes] = gearoenix::platform::BaseApplication::get_default_font_data();
    if (nullptr == content || read_bytes < 1) {
        return;
    }
    auto& io = ImGui::GetIO();
    auto* const fonts = io.Fonts;
    io.FontDefault = fonts->AddFontFromMemoryTTF(content, read_bytes, 15);
}

void register_types()
{
}

void platform_set_ime_data(ImGuiContext*, ImGuiViewport*, ImGuiPlatformImeData* const data)
{
    if (!data->WantVisible && !data->WantTextInput) {
        gearoenix::platform::Application::get().stop_keyboard_capture();
    }
    if (data->WantVisible) {
        gearoenix::platform::Application::get().set_text_input_area(
            static_cast<int>(data->InputPos.x),
            static_cast<int>(data->InputPos.y),
            1,
            static_cast<int>(data->InputLineHeight));
    }
    if (data->WantVisible || data->WantTextInput) {
        gearoenix::platform::Application::get().start_keyboard_capture();
    }
}

const char* get_clipboard_text(ImGuiContext*)
{
    return gearoenix::platform::Application::get().get_clipboard();
}

void set_clipboard_text(ImGuiContext* const, const char* text)
{
    gearoenix::platform::Application::get().set_clipboard(text);
}
}

void gearoenix::platform::BaseApplication::initialise_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    auto& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.BackendPlatformName = RuntimeConfiguration::get().get_application_name().c_str();

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Platform_SetClipboardTextFn = set_clipboard_text;
    platform_io.Platform_GetClipboardTextFn = get_clipboard_text;
    platform_io.Platform_SetImeDataFn = platform_set_ime_data;
    platform_io.Platform_OpenInShellFn = [](ImGuiContext*, const char* const url) {
        return !Application::get().open_url(url);
    };
}

gearoenix::platform::BaseApplication::BaseApplication()
    : Singleton(this)
    , event_engine(new core::event::Engine())
    , net_manager(new net::Manager())
    , should_window_be_closed([this] {
        static constexpr char name[] = "Quit Gearoenix application?";
        static constexpr char body[] = "Are you sure you want to quit this Gearoenix instance?\nYou will loose your current unsaved progress in your game or work!";
        static const std::function<void()> fun = [this] {
            running = false;
        };

        render::imgui::show_sure_popup(name, window_is_going_to_be_closed, body, fun);

        return !running;
    })
{
    register_types();

    GX_LOG_D("This machine has " << core::sync::threads_count() << " number of thread cores.");
    core::job::initialise();

    initialise_imgui();

    if (const auto& config = RuntimeConfiguration::get(); !config.get_fullscreen()) {
        initialize_window_size(
            static_cast<int>(config.get_window_width()),
            static_cast<int>(config.get_window_height()));
    }
}

gearoenix::platform::BaseApplication::~BaseApplication() = default;

void gearoenix::platform::BaseApplication::initialize_window_position(const int x, const int y)
{
    window_size.x = x;
    window_size.y = y;
}

void gearoenix::platform::BaseApplication::initialize_window_size(const int w, const int h)
{
    update_window_size(w, h);
    previous_window_size.x = w;
    previous_window_size.y = h;
    window_resizing = false;
}

void gearoenix::platform::BaseApplication::update_window_size(const int w, const int h)
{
    const auto new_size = decltype(window_size) { w, h };
    if (window_size == new_size) {
        return;
    }

    window_resizing = true;
    window_size = new_size;

    auto& io = ImGui::GetIO();
    io.DisplaySize = {
        static_cast<decltype(io.DisplaySize.x)>(window_size.x),
        static_cast<decltype(io.DisplaySize.y)>(window_size.y),
    };

    last_time_window_resized = std::chrono::high_resolution_clock::now();
}

void gearoenix::platform::BaseApplication::update_window()
{
    if (const auto& config = RuntimeConfiguration::get();
        window_resizing && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - last_time_window_resized).count() > config.get_window_resizing_event_interval_ms()) {
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

void gearoenix::platform::BaseApplication::initialize_mouse_position(const double x, const double y)
{
    mouse_previous_position = mouse_position = { x, y };
    mouse_previous_normalised_position = mouse_normalised_position = normalise_position(x, y);
    ImGui::GetIO().MousePos = { static_cast<float>(x), static_cast<float>(y) };
}

void gearoenix::platform::BaseApplication::update_mouse_position(const double x, const double y)
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

void gearoenix::platform::BaseApplication::mouse_key(const key::Id k, const key::Action a)
{
    ImGui::GetIO().MouseDown[key::convert_mouse_to_imgui(k)] = a == key::Action::Press;
    event_engine->broadcast(
        core::event::Data(
            core::event::Id::ButtonMouse,
            core::event::button::Mouse(
                a, k, mouse_normalised_position, mouse_position)));
}

void gearoenix::platform::BaseApplication::mouse_wheel(const double v)
{
    ImGui::GetIO().MouseWheel += static_cast<float>(v);
}

void gearoenix::platform::BaseApplication::keyboard_key(const key::Id k, const key::Action a)
{
    const auto pressed = a == key::Action::Press;
    auto& io = ImGui::GetIO();
    io.AddKeyEvent(static_cast<ImGuiKey>(key::convert_keyboard_to_imgui(k)), pressed);
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

void gearoenix::platform::BaseApplication::character_input(const char16_t ch)
{
    ImGui::GetIO().AddInputCharacterUTF16(ch);
}

void gearoenix::platform::BaseApplication::touch_down(const FingerId finger_id, const double x, const double y)
{
    const core::event::Point2D p({ x, y }, normalise_position(x, y));
    touch_states[finger_id] = p;
    event_engine->broadcast(core::event::Data(core::event::Id::Touch, core::event::touch::Data(p, finger_id, core::event::touch::Action::Down)));
}

void gearoenix::platform::BaseApplication::touch_move(const FingerId finger_id, const double x, const double y)
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

void gearoenix::platform::BaseApplication::touch_up(const FingerId finger_id)
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

void gearoenix::platform::BaseApplication::touch_cancel(const FingerId finger_id)
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

void gearoenix::platform::BaseApplication::initialize_engine()
{
    initialise_default_font();
    render_engine = render::engine::Engine::construct();
    audio_engine = std::make_unique<audio::Engine>();
}

void gearoenix::platform::BaseApplication::initialize_core_application(core::Application* const core_app)
{
    if (nullptr == core_app) {
        core_application = std::make_unique<core::Application>();
    } else {
        core_application = std::unique_ptr<core::Application>(core_app);
    }
}

void gearoenix::platform::BaseApplication::close()
{
    running = false;
}

void gearoenix::platform::BaseApplication::terminate()
{
    running = false;
    core_application = nullptr;
    core::ecs::World::get().clear();
    render_engine = nullptr;
    ImGui::DestroyContext();
    core::job::terminate();
}

void gearoenix::platform::BaseApplication::update()
{
    update_window();
    render_engine->start_frame();

    if (window_is_going_to_be_closed) {
        running = !should_window_be_closed();
    }

    file_chooser_update();
    core_application->update();
    audio_engine->update();
    render_engine->end_frame();
}

std::pair<void*, int> gearoenix::platform::BaseApplication::get_default_font_data()
{
    const auto font = stream::Stream::open(stream::Path::create_asset("default-font.ttf"));
    if (nullptr == font) {
        return {nullptr, 0};
    }
    const auto content_sz = font->size();
    auto* const content = new std::uint8_t[content_sz];
    const auto read_bytes = font->read(content, content_sz);
    GX_ASSERT_D(content_sz == read_bytes);
    return { content, static_cast<int>(read_bytes) };
}

double gearoenix::platform::BaseApplication::normalise_x(const double x) const
{
    return (x - static_cast<double>(window_size.x) * 0.5) / static_cast<double>(window_size.y);
}

double gearoenix::platform::BaseApplication::normalise_y(const double y) const
{
    return 0.5 - (y / static_cast<double>(window_size.y));
}

gearoenix::math::Vec2<double> gearoenix::platform::BaseApplication::normalise_position(const double x, const double y) const
{
    return { normalise_x(x), normalise_y(y) };
}
