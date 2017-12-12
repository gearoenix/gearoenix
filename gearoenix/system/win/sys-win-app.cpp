#include "sys-win-app.hpp"
#ifdef USE_WINAPI
//#define GEAROENIX_FULLSCREEN
//#define GEAROENIX_NO_CURSOR
#include "../../core/cr-static.hpp"
#include "../sys-log.hpp"
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

gearoenix::system::Application::Application()
{
	WNDCLASSEX wc;
	DEVMODE screen_settings;
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
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
#ifdef GEAROENIX_FULLSCREEN
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
}
#endif // USE_WINAPI