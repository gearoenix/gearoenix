#include "gx-plt-win-application.hpp"
#ifdef GX_PLATFORM_INTERFACE_WIN32
#include "../../core/gx-cr-application.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-plt-log.hpp"
#include "gx-plt-win-key.hpp"
#include <Windows.h>

LRESULT CALLBACK gearoenix::platform::Application::static_handler(
    HWND hwnd,
    const UINT message,
    const WPARAM w_param,
    const LPARAM l_param) noexcept
{
    auto platform_application = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (WM_CREATE == message) {
        auto* const create_structure = reinterpret_cast<CREATESTRUCT*>(l_param);
        platform_application = static_cast<Application*>(create_structure->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(platform_application));
    }
    if (nullptr == platform_application) {
        GX_LOG_D("Unhandled message for nullptr interface, uMsg is: " << message)
        return DefWindowProc(hwnd, message, w_param, l_param);
    }
    return platform_application->handler(hwnd, message, w_param, l_param);
}

LRESULT gearoenix::platform::Application::handler(
    HWND hwnd,
    const UINT message,
    const WPARAM w_param,
    const LPARAM l_param) noexcept
{
    switch (message) {
    case WM_CLOSE:
        base.going_to_be_closed();
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        ValidateRect(hwnd, nullptr);
        break;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        base.keyboard_key(convert_to_keyboard_key(w_param, l_param), key::Action::Press);
        break;
    case WM_SYSKEYUP:
    case WM_KEYUP:
        base.keyboard_key(convert_to_keyboard_key(w_param, l_param), key::Action::Release);
        break;
    case WM_CHAR:
        base.character_input(static_cast<char16_t>(w_param));
        break;
    case WM_LBUTTONDOWN:
        base.mouse_key(key::Id::Left, key::Action::Press);
        break;
    case WM_RBUTTONDOWN:
        base.mouse_key(key::Id::Right, key::Action::Press);
        break;
    case WM_MBUTTONDOWN:
        base.mouse_key(key::Id::Middle, key::Action::Press);
        break;
    case WM_LBUTTONUP:
        base.mouse_key(key::Id::Left, key::Action::Release);
        break;
    case WM_RBUTTONUP:
        base.mouse_key(key::Id::Right, key::Action::Release);
        break;
    case WM_MBUTTONUP:
        base.mouse_key(key::Id::Middle, key::Action::Release);
        break;
    case WM_MOUSEWHEEL:
        base.mouse_wheel(static_cast<double>(GET_WHEEL_DELTA_WPARAM(w_param)) / static_cast<double>(WHEEL_DELTA));
        break;
    case WM_MOUSEMOVE:
        update_mouse_position();
        break;
    case WM_SIZE:
        if (w_param == SIZE_MINIMIZED)
            break;
        update_window_size();
        break;
    case WM_SHOWWINDOW:
        base.window_is_up = true;
        break;
    case WM_ENTERSIZEMOVE:
        base.window_resizing = true;
        break;
    default:
        // GX_LOG_D("Unhandled message: " << message)
        break;
    }
    return DefWindowProc(hwnd, message, w_param, l_param);
}

void gearoenix::platform::Application::update_mouse_position() noexcept
{
    POINT p;
    if (GetCursorPos(&p) && ScreenToClient(window, &p)) {
        base.update_mouse_position(p.x, p.y);
    }
}

void gearoenix::platform::Application::update_window_size() noexcept
{
    if (!base.window_is_up)
        return;
    RECT rcc;
    GetClientRect(window, &rcc);
    const int w = rcc.right - rcc.left;
    if (0 >= w)
        return;
    const int h = rcc.bottom - rcc.top;
    if (0 >= h)
        return;
    base.update_window_size(w, h);
}

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
    DWORD style = WS_VISIBLE;
    if (config.get_fullscreen()) {
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
        style |= WS_OVERLAPPEDWINDOW;
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
    POINT p;
    if (GetCursorPos(&p) && ScreenToClient(window, &p))
        base.initialize_mouse_position(p.x, p.y);
    RECT rcc;
    if (GetClientRect(window, &rcc)) {
        const int w = rcc.right - rcc.left;
        const int h = rcc.bottom - rcc.top;
        if (0 < w && 0 < h) {
            base.initialize_window_size(w, h);
        }
    }
    base.initialize_engine(*this);
}

gearoenix::platform::Application::~Application() noexcept = default;

void gearoenix::platform::Application::run(core::Application* core_app) noexcept
{
    base.initialize_core_application(this, core_app);
    GX_LOG_D("Run function of WinApi interface called.")
    MSG msg;
    while (base.running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        base.update();
    }
}

#endif
