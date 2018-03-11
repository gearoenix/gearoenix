#include "sys-win-app.hpp"
#ifdef USE_WINAPI
//#define GEAROENIX_FULLSCREEN
//#define GEAROENIX_NO_CURSOR
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-bt-keyboard.hpp"
#include "../../core/event/cr-ev-bt-mouse.hpp"
#include "../../core/event/cr-ev-mv-mouse.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../sys-log.hpp"
#ifdef USE_VULKAN
#include "../../vulkan/vk-engine.hpp"
#endif
#ifdef USE_DIRECTX12
#include "../../dx12/dx12-engine.hpp"
#endif
#ifdef USE_DIRECTX11
#include "../../dx11/dx11-engine.hpp"
#endif
#ifdef USE_OPENGL_41
#include "../../gl41/gl41-engine.hpp"
#endif
#ifdef USE_OPENGL_33
#include "../../gl31/gl31-engine.hpp"
#endif
LRESULT CALLBACK gearoenix::system::Application::wnd_proc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    auto sys_app = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (WM_CREATE == umessage) {
        CREATESTRUCT* create_structure = reinterpret_cast<CREATESTRUCT*>(lparam);
        sys_app = reinterpret_cast<Application*>(create_structure->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(sys_app));
    }
    if (nullptr == sys_app) {
        GXLOGE("Unexpected message for nullptr sys app uMsg is: " << umessage);
        return (DefWindowProc(hwnd, umessage, wparam, lparam));
    }
    return sys_app->handler(hwnd, umessage, wparam, lparam);
}

LRESULT CALLBACK gearoenix::system::Application::handler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	core::event::Event *event = nullptr;
    switch (umessage) {
    case WM_CLOSE:
        /// TODO: proper termination
        running = false;
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        ValidateRect(hwnd, NULL);
        break;
    case WM_KEYDOWN:
        switch (wparam) {
		case VK_UP: {
			event = new core::event::button::Keyboard(
				core::event::button::Button::KeyType::UP, 
				core::event::button::Button::ActionType::PRESS);
			break;
		}
        case 0x50: /// p
            /// TODO pause
            break;
        case VK_F1:
            /*if (enableTextOverlay) {
			textOverlay->visible = !textOverlay->visible;
			}*/
            break;
        case VK_ESCAPE:
            running = false;
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            break;
        }
        /*if (camera.firstperson) {
		switch (wParam)
		{
		case KEY_W:
		camera.keys.up = true;
		break;
		case KEY_S:
		camera.keys.down = true;
		break;
		case KEY_A:
		camera.keys.left = true;
		break;
		case KEY_D:
		camera.keys.right = true;
		break;
		}
		}*/
        break;
    case WM_KEYUP:
        // if (camera.firstperson) {
        //	switch (wParam)
        //	{
        //	case 0x57: // W
        //		camera.keys.up = false;
        //		break;
        //	case 0x53: // S
        //		camera.keys.down = false;
        //		break;
        //	case 0x41: // A
        //		camera.keys.left = false;
        //		break;
        //	case 0x44: // D
        //		camera.keys.right = false;
        //		break;
        //	}
        //}
        break;
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
        update_mouse_position();
        core::event::button::Button::KeyType k;
        switch (umessage) {
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            k = core::event::button::Button::KeyType::LEFT;
            break;
        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            k = core::event::button::Button::KeyType::RIGHT;
            break;
        case WM_MBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            k = core::event::button::Button::KeyType::MIDDLE;
            break;
        default:
            k = core::event::button::Button::KeyType::LEFT;
            break;
        }
        core::event::button::Button::ActionType a;
        switch (umessage) {
        case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
            a = core::event::button::Button::ActionType::DOUBLE;
            break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            a = core::event::button::Button::ActionType::PRESS;
            break;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            a = core::event::button::Button::ActionType::RELEASE;
            break;
        default:
            a = core::event::button::Button::ActionType::PRESS;
            break;
        }
        core::event::button::Mouse e(k, a, mouse_x, mouse_y);
        render_engine->on_event(e);
        core_app->on_event(e);
        break;
    }
    case WM_MOUSEWHEEL:
        //core_app->on_scroll(((core::Real)GET_WHEEL_DELTA_WPARAM(wparam)) * 0.01f);
        break;
    case WM_MOUSEMOVE: {
        core::Real pre_x = mouse_x;
        core::Real pre_y = mouse_y;
        update_mouse_position();
        core::event::movement::Mouse e(mouse_x, mouse_y, pre_x, pre_y);
        render_engine->on_event(e);
        core_app->on_event(e);
        break;
    }
    case WM_SIZE: {
        if (!window_is_up || wparam == SIZE_MINIMIZED)
            break;
        if (!resizing && wparam != SIZE_MAXIMIZED && wparam != SIZE_RESTORED)
            break;
        core::Real pre_screen_width = (core::Real)screen_width;
        core::Real pre_screen_height = (core::Real)screen_height;
        update_screen_sizes();
        core::event::WindowResize e(
            pre_screen_width, pre_screen_height,
            (core::Real)screen_width, (core::Real)screen_height);
        render_engine->on_event(e);
        break;
    }
    case WM_SHOWWINDOW:
        window_is_up = true;
        break;
    case WM_ENTERSIZEMOVE:
        resizing = true;
        break;
    case WM_EXITSIZEMOVE:
        resizing = false;
        break;
    }
	if (event != nullptr) {
		render_engine->on_event(*event);
		core_app->on_event(*event);
		delete event;
	}
    return (DefWindowProc(hwnd, umessage, wparam, lparam));
}

gearoenix::core::Real gearoenix::system::Application::pixel_to_normal_pos_x(int x) const
{
    return (((core::Real)x) * half_height_inversed) - screen_ratio;
}

gearoenix::core::Real gearoenix::system::Application::pixel_to_normal_pos_y(int y) const
{
    return 1.0f - (((core::Real)y) * half_height_inversed);
}

void gearoenix::system::Application::update_mouse_position()
{
    POINT p;
    if (GetCursorPos(&p)) {
        if (ScreenToClient(window, &p)) {
            mouse_x_pixel = p.x;
            mouse_y_pixel = p.y;
            mouse_x = pixel_to_normal_pos_x(mouse_x_pixel);
            mouse_y = pixel_to_normal_pos_y(mouse_y_pixel);
        }
    }
}

void gearoenix::system::Application::update_screen_sizes()
{
    RECT rcc, rcw;
    GetClientRect(window, &rcc);
    GetWindowRect(window, &rcw);
    screen_width = rcc.right - rcc.left;
    screen_height = rcc.bottom - rcc.top;
    border_width = ((rcw.right - rcw.left) - screen_width) / 2;
    title_bar_height = ((rcw.bottom - rcw.top) - screen_height) - border_width;
    half_height_inversed = 2.0f / (core::Real)screen_height;
    screen_ratio = (core::Real)screen_width / (core::Real)screen_height;
}

gearoenix::system::Application::Application()
{
    WNDCLASSEX wc;
    int pos_x, pos_y;
    instance = GetModuleHandle(NULL);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = wnd_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = APPLICATION_NAME;
    wc.cbSize = sizeof(WNDCLASSEX);
    RegisterClassEx(&wc);
    screen_width = GetSystemMetrics(SM_CXSCREEN);
    screen_height = GetSystemMetrics(SM_CYSCREEN);
#ifdef GEAROENIX_FULLSCREEN
    DEVMODE screen_settings;
    GXSETZ(screen_settings);
    screen_settings.dmSize = sizeof(screen_settings);
    screen_settings.dmPelsWidth = (unsigned long)screen_width;
    screen_settings.dmPelsHeight = (unsigned long)screen_height;
    screen_settings.dmBitsPerPel = 32;
    screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
    pos_x = pos_y = 0;
#else
    screen_width = GEAROENIX_DEFAULT_WINDOW_WIDTH;
    screen_height = GEAROENIX_DEFAULT_WINDOW_HEIGHT;
    pos_x = (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2;
    pos_y = (GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2;
#endif
    window = CreateWindowEx(WS_EX_APPWINDOW, APPLICATION_NAME, APPLICATION_NAME,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
#ifndef GEAROENIX_FULLSCREEN
            | WS_OVERLAPPEDWINDOW
#endif

        ,
        pos_x, pos_y, screen_width, screen_height, NULL, NULL, instance, this);
    ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window);
    SetFocus(window);
    UpdateWindow(window);
#ifdef GEAROENIX_NO_CURSOR
    ShowCursor(false);
#endif
    while (!window_is_up) {
        MSG msg;
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    update_screen_sizes();
    update_mouse_position();
#ifdef USE_VULKAN
    if (vulkan::Engine::is_supported())
        render_engine = new vulkan::Engine(this);
    else
#endif
#ifdef USE_DIRECTX12
        if (dx12::Engine::is_supported())
        render_engine = new dx12::Engine(this);
    else
#endif
#ifdef USE_DIRECTX11
        if (dx11::Engine::is_supported())
        render_engine = new dx11::Engine(this);
    else
#endif
#ifdef USE_OPENGL_41
        if (gl41::Engine::is_supported())
        render_engine = new gl41::Engine(this);
    else
#endif
#ifdef USE_OPENGL_33
        if (gl33::Engine::is_supported())
        render_engine = new gl33::Engine(this);
    else
#endif
    {
        GXLOGF("No suitable API found.");
    }
    astmgr = new core::asset::Manager(this, "data.gx3d");
    astmgr->initialize();
}

gearoenix::system::Application::~Application()
{
    TODO; // do cleaning, I think it is better to do it in some special event instead of here.
}

void gearoenix::system::Application::execute(core::Application* core_app)
{
    this->core_app = core_app;
    MSG msg;
    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        core_app->update();
        render_engine->update();
    }
    delete core_app;
    core_app = nullptr;
    delete render_engine;
    render_engine = nullptr;
}

gearoenix::render::Engine* gearoenix::system::Application::get_render_engine()
{
    return render_engine;
}

const gearoenix::render::Engine* gearoenix::system::Application::get_render_engine() const
{
    return render_engine;
}

gearoenix::core::Application* gearoenix::system::Application::get_core_app()
{
    return core_app;
}

const gearoenix::core::Application* gearoenix::system::Application::get_core_app() const
{
    return core_app;
}

gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager()
{
    return astmgr;
}

const gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager() const
{
    return astmgr;
}

gearoenix::core::Real gearoenix::system::Application::get_window_ratio() const
{
    return ((core::Real)screen_width) / ((core::Real)screen_height);
}

unsigned int gearoenix::system::Application::get_width() const
{
    return (unsigned int)screen_width;
}

unsigned int gearoenix::system::Application::get_height() const
{
    return (unsigned int)screen_height;
}

HWND gearoenix::system::Application::get_window()
{
    return window;
}

#endif // USE_WINAPI
