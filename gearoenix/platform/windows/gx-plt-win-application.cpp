#include "gx-plt-win-application.hpp"
#ifdef GX_PLATFORM_WINDOWS
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-plt-log.hpp"
#include <Windows.h>

LRESULT CALLBACK gearoenix::platform::Application::static_handler(
    HWND hwnd,
    const UINT message,
    const WPARAM w_param,
    const LPARAM l_param) noexcept
{
    auto sys_app = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (WM_CREATE == message) {
        auto* const create_structure = reinterpret_cast<CREATESTRUCT*>(l_param);
        sys_app = static_cast<Application*>(create_structure->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(sys_app));
    }
    if (nullptr == sys_app) {
        GX_LOG_D("Unhandled message for nullptr interface, uMsg is: " << message)
        return DefWindowProc(hwnd, message, w_param, l_param);
    }
    return sys_app->handler(hwnd, message, w_param, l_param);
}

LRESULT gearoenix::platform::Application::handler(
    HWND hwnd,
    const UINT message,
    const WPARAM w_param,
    const LPARAM l_param) noexcept
{
    //    core::event::Event* event = nullptr;
    switch (message) {
    case WM_CLOSE:
        base.going_to_be_closed();
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        ValidateRect(hwnd, nullptr);
        break;
        //    case WM_KEYDOWN:
        //        switch (wparam) {
        //        case VK_DOWN:
        //            event = new core::event::button::Keyboard(
        //                core::event::button::Button::KeyType::DOWN,
        //                core::event::button::Button::ActionType::PRESS);
        //            break;
        //        case VK_LEFT:
        //            event = new core::event::button::Keyboard(
        //                core::event::button::Button::KeyType::LEFT,
        //                core::event::button::Button::ActionType::PRESS);
        //            break;
        //        case VK_RIGHT:
        //            event = new core::event::button::Keyboard(
        //                core::event::button::Button::KeyType::RIGHT,
        //                core::event::button::Button::ActionType::PRESS);
        //            break;
        //        case VK_UP:
        //            event = new core::event::button::Keyboard(
        //                core::event::button::Button::KeyType::UP,
        //                core::event::button::Button::ActionType::PRESS);
        //            break;
        //        case VK_F1:
        //            //event = new core::event::button::Keyboard(
        //            //	core::event::button::Button::KeyType::F1,
        //            //	core::event::button::Button::ActionType::PRESS);
        //            break;
        //        case VK_ESCAPE:
        //            running = false;
        //            DestroyWindow(hwnd);
        //            PostQuitMessage(0);
        //            break;
        //        default:
        //            break;
        //        }
        //        break;
        //    case WM_KEYUP:
        //        break;
        //    case WM_LBUTTONDBLCLK:
        //    case WM_MBUTTONDBLCLK:
        //    case WM_RBUTTONDBLCLK:
        //    case WM_LBUTTONDOWN:
        //    case WM_MBUTTONDOWN:
        //    case WM_RBUTTONDOWN:
        //    case WM_LBUTTONUP:
        //    case WM_MBUTTONUP:
        //    case WM_RBUTTONUP: {
        //        update_mouse_position();
        //        core::event::button::Button::KeyType k;
        //        switch (umessage) {
        //        case WM_LBUTTONDBLCLK:
        //        case WM_LBUTTONDOWN:
        //        case WM_LBUTTONUP:
        //            k = core::event::button::Button::KeyType::LEFT;
        //            break;
        //        case WM_RBUTTONDBLCLK:
        //        case WM_RBUTTONDOWN:
        //        case WM_RBUTTONUP:
        //            k = core::event::button::Button::KeyType::RIGHT;
        //            break;
        //        case WM_MBUTTONDBLCLK:
        //        case WM_MBUTTONDOWN:
        //        case WM_MBUTTONUP:
        //            k = core::event::button::Button::KeyType::MIDDLE;
        //            break;
        //        default:
        //            k = core::event::button::Button::KeyType::LEFT;
        //            break;
        //        }
        //        core::event::button::Button::ActionType a;
        //        switch (umessage) {
        //        case WM_LBUTTONDBLCLK:
        //        case WM_MBUTTONDBLCLK:
        //        case WM_RBUTTONDBLCLK:
        //            a = core::event::button::Button::ActionType::DOUBLE;
        //            break;
        //        case WM_LBUTTONDOWN:
        //        case WM_MBUTTONDOWN:
        //        case WM_RBUTTONDOWN:
        //            a = core::event::button::Button::ActionType::PRESS;
        //            break;
        //        case WM_LBUTTONUP:
        //        case WM_MBUTTONUP:
        //        case WM_RBUTTONUP:
        //            a = core::event::button::Button::ActionType::RELEASE;
        //            break;
        //        default:
        //            a = core::event::button::Button::ActionType::PRESS;
        //            break;
        //        }
        //        core::event::button::Mouse e(k, a, mouse_x, mouse_y);
        //        render_engine->on_event(e);
        //        core_app->on_event(e);
        //        break;
        //    }
        //    case WM_MOUSEWHEEL:
        //        //core_app->on_scroll(((core::Real)GET_WHEEL_DELTA_WPARAM(wparam)) * 0.01f);
        //        break;
        //    case WM_MOUSEMOVE: {
        //        core::Real pre_x = mouse_x;
        //        core::Real pre_y = mouse_y;
        //        update_mouse_position();
        //        core::event::movement::Mouse e(mouse_x, mouse_y, pre_x, pre_y);
        //        render_engine->on_event(e);
        //        core_app->on_event(e);
        //        break;
        //    }
    case WM_SIZE: {
        if (!base.window_is_up || w_param == SIZE_MINIMIZED)
            break;
        //        core::Real pre_screen_width = (core::Real)screen_width;
        //        core::Real pre_screen_height = (core::Real)screen_height;
        //        update_screen_sizes();
        //        core::event::WindowResize e(
        //            pre_screen_width, pre_screen_height,
        //            (core::Real)screen_width, (core::Real)screen_height);
        //        render_engine->on_event(e);
        break;
    }
    case WM_SHOWWINDOW:
        base.window_is_up = true;
        break;
        //    case WM_ENTERSIZEMOVE:
        //        resizing = true;
        //        break;
        //    case WM_EXITSIZEMOVE:
        //        resizing = false;
        //        break;
    default:
        GX_LOG_D("Unhandled message: " << message)
    }
    //    if (event != nullptr) {
    //        render_engine->on_event(*event);
    //        core_app->on_event(*event);
    //        delete event;
    //    }
    return DefWindowProc(hwnd, message, w_param, l_param);
}

//void gearoenix::platform::Application::update_mouse_position()
//{
//    POINT p;
//    if (GetCursorPos(&p)) {
//        if (ScreenToClient(window, &p)) {
//            mouse_x_pixel = p.x;
//            mouse_y_pixel = p.y;
//            mouse_x = pixel_to_normal_pos_x(mouse_x_pixel);
//            mouse_y = pixel_to_normal_pos_y(mouse_y_pixel);
//        }
//    }
//}
//
//void gearoenix::platform::Application::update_screen_sizes()
//{
//    RECT rcc, rcw;
//    GetClientRect(window, &rcc);
//    GetWindowRect(window, &rcw);
//    screen_width = rcc.right - rcc.left;
//    screen_height = rcc.bottom - rcc.top;
//    border_width = ((rcw.right - rcw.left) - screen_width) / 2;
//    title_bar_height = ((rcw.bottom - rcw.top) - screen_height) - border_width;
//    half_height_inversed = 2.0f / (core::Real)screen_height;
//    screen_ratio = (core::Real)screen_width / (core::Real)screen_height;
//}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    WNDCLASSEXA wc;
    GX_SET_ZERO(wc)
    instance = GetModuleHandle(nullptr);
    wc.style = static_cast<UINT>(CS_HREDRAW) | static_cast<UINT>(CS_VREDRAW) | static_cast<UINT>(CS_OWNDC);
    wc.lpfnWndProc = static_handler;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = config.get_application_name().c_str();
    wc.cbSize = sizeof(WNDCLASSEXA);
    RegisterClassExA(&wc);
    DWORD style = static_cast<DWORD>(WS_VISIBLE) | static_cast<DWORD>(WS_OVERLAPPEDWINDOW);
    if (config.get_fullscreen()) {
        base.initialize_window_size(
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN));
        DEVMODE screen_settings;
        GX_SET_ZERO(screen_settings)
        screen_settings.dmSize = sizeof(screen_settings);
        screen_settings.dmPelsWidth = static_cast<decltype(screen_settings.dmPelsWidth)>(base.window_width);
        screen_settings.dmPelsHeight = static_cast<decltype(screen_settings.dmPelsHeight)>(base.window_height);
        screen_settings.dmBitsPerPel = 32;
        screen_settings.dmFields = static_cast<DWORD>(DM_BITSPERPEL) | static_cast<DWORD>(DM_PELSWIDTH) | static_cast<DWORD>(DM_PELSHEIGHT);
        ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
        base.initialize_window_position(0, 0);
        style |= WS_POPUP;
    } else {
        base.initialize_window_position(
            (GetSystemMetrics(SM_CXSCREEN) - base.window_width) / 2,
            (GetSystemMetrics(SM_CYSCREEN) - base.window_height) / 2);
        style |= static_cast<DWORD>(WS_CAPTION) | static_cast<DWORD>(WS_SYSMENU) | static_cast<DWORD>(WS_THICKFRAME) | static_cast<DWORD>(WS_MINIMIZEBOX) | static_cast<DWORD>(WS_MAXIMIZEBOX);
    }
    window = CreateWindowExA(
        WS_EX_APPWINDOW, GX_APPLICATION_NAME, GX_APPLICATION_NAME, style,
        base.window_x, base.window_y, base.window_width, base.window_height,
        nullptr, nullptr, instance, this);
    ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window);
    SetFocus(window);
    UpdateWindow(window);
    ShowCursor(config.get_show_cursor());
    while (!base.window_is_up) {
        MSG msg;
        GetMessage(&msg, nullptr, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    //    update_screen_sizes();
    //    update_mouse_position();
}

std::shared_ptr<gearoenix::platform::Application> gearoenix::platform::Application::construct(
    GX_MAIN_ENTRY_ARGS_DEF,
    const RuntimeConfiguration& config) noexcept
{
    std::shared_ptr<Application> result(new Application(GX_MAIN_ENTRY_ARGS, config));
    result->self = result;
    result->base.render_engine = render::engine::Engine::construct(config.get_render_configuration(), result);
    GX_TODO
    //    astmgr = new core::asset::Manager(this, "data.gx3d");
    //    astmgr->initialize();
    //    astmgr->set_render_engine(render_engine);

    return result;
}

gearoenix::platform::Application::~Application() noexcept = default;

void gearoenix::platform::Application::run() noexcept
{
    GX_LOG_D("Run function of WinApi interface called.")
    //    this->core_app = core_app;
    MSG msg;
    while (base.running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //        core_app->update();
        //        render_engine->update();
    }
    //    core_app = nullptr;
    //    render_engine = nullptr;
}

#endif
