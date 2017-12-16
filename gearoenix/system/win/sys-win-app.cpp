#include "sys-win-app.hpp"
#ifdef USE_WINAPI
//#define GEAROENIX_FULLSCREEN
//#define GEAROENIX_NO_CURSOR
#include "../../core/cr-static.hpp"
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
        case 0x50: /// p
            /// TODO pause
            break;
        case VK_F1:
            /*if (enableTextOverlay) {
			textOverlay->visible = !textOverlay->visible;
			}*/
            break;
        case VK_ESCAPE:
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
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
        // mousePos.x = (float)LOWORD(lParam);
        // mousePos.y = (float)HIWORD(lParam);
        break;
    case WM_MOUSEWHEEL: {
        // short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        // zoom += (float)wheelDelta * 0.005f * zoomSpeed;
        // camera.translate(glm::vec3(0.0f, 0.0f, (float)wheelDelta * 0.005f *
        // zoomSpeed));
        // viewUpdated = true;
        break;
    }
    case WM_MOUSEMOVE:
        if (wparam & MK_RBUTTON) {
            // int32_t posx = LOWORD(lParam);
            // int32_t posy = HIWORD(lParam);
            // zoom += (mousePos.y - (float)posy) * .005f * zoomSpeed;
            // camera.translate(glm::vec3(-0.0f, 0.0f, (mousePos.y - (float)posy) *
            // .005f * zoomSpeed));
            // mousePos = glm::vec2((float)posx, (float)posy);
            // viewUpdated = true;
        }
        if (wparam & MK_LBUTTON) {
            // int32_t posx = LOWORD(lParam);
            // int32_t posy = HIWORD(lParam);
            // rotation.x += (mousePos.y - (float)posy) * 1.25f * rotationSpeed;
            // rotation.y -= (mousePos.x - (float)posx) * 1.25f * rotationSpeed;
            // camera.rotate(glm::vec3((mousePos.y - (float)posy) *
            // camera.rotationSpeed, -(mousePos.x - (float)posx) *
            // camera.rotationSpeed, 0.0f));
            // mousePos = glm::vec2((float)posx, (float)posy);
            // viewUpdated = true;
        }
        if (wparam & MK_MBUTTON) {
            // int32_t posx = LOWORD(lParam);
            // int32_t posy = HIWORD(lParam);
            // cameraPos.x -= (mousePos.x - (float)posx) * 0.01f;
            // cameraPos.y -= (mousePos.y - (float)posy) * 0.01f;
            // camera.translate(glm::vec3(-(mousePos.x - (float)posx) * 0.01f,
            // -(mousePos.y - (float)posy) * 0.01f, 0.0f));
            // viewUpdated = true;
            // mousePos.x = (float)posx;
            // mousePos.y = (float)posy;
        }
        break;
    case WM_SIZE:
        // if ((prepared) && (wParam != SIZE_MINIMIZED))
        //{
        //	if ((resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam ==
        // SIZE_RESTORED)))
        //	{
        //		destWidth = LOWORD(lParam);
        //		destHeight = HIWORD(lParam);
        //		windowResize();
        //	}
        //}
        break;
    case WM_SHOWWINDOW:
        window_is_up = true;
        break;
    case WM_ENTERSIZEMOVE:
        // resizing = true;
        break;
    case WM_EXITSIZEMOVE:
        // resizing = false;
        break;
    }
    return (DefWindowProc(hwnd, umessage, wparam, lparam));
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
    wc.hIcon = NULL;
    wc.hIconSm = NULL;
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
    setz(screen_settings);
    screen_settings.dmSize = sizeof(screen_settings);
    screen_settings.dmPelsWidth = (unsigned long)screen_width;
    screen_settings.dmPelsHeight = (unsigned long)screen_height;
    screen_settings.dmBitsPerPel = 32;
    screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
    posX = posY = 0;
#else
    screen_width = DEFAULT_WINDOW_WIDTH;
    screen_height = DEFAULT_WINDOW_HEIGHT;
    pos_x = (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2;
    pos_y = (GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2;
#endif
    window = CreateWindowEx(WS_EX_APPWINDOW, APPLICATION_NAME, APPLICATION_NAME,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
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
    //astmgr = new core::asset::Manager(this, "data.gx3d");
    //astmgr->initialize();
}

gearoenix::system::Application::~Application()
{
    TODO; // do cleaning, I think it is better to do it in some special event in stead of here.
}

void gearoenix::system::Application::execute(core::Application* core_app)
{
    MSG msg;
    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //core_app->update();
        render_engine->update();
    }
}

gearoenix::render::Engine* gearoenix::system::Application::get_render_engine()
{
    return nullptr;
}

const gearoenix::render::Engine* gearoenix::system::Application::get_render_engine() const
{
    return nullptr;
}

gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager()
{
    return nullptr;
}

const gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager() const
{
    return nullptr;
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

#endif // USE_WINAPI

HWND gearoenix::system::Application::get_window()
{
    return window;
}
