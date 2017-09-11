#include "win-app.hpp"
#ifdef IN_WINDOWS
#ifdef DEBUG_MODE
#define WIN_DEBUG
#endif
#include "../../core/static.hpp"
#include "../log.hpp"
#include "../../core/application.hpp"
#include <string>
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	std::shared_ptr<gearoenix::nufrag::system::Application> app(
		new gearoenix::nufrag::system::Application(hInstance, hPrevInstance, pCmdLine, nCmdShow));
	return app->execute();
}
int gearoenix::nufrag::system::Application::execute() {
	MSG msg;
	while (running) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		core_app->update();
		render_engine->update();
	}
	return (int)msg.wParam;
}

const HINSTANCE & gearoenix::nufrag::system::Application::get_hinstance() const {
	return instance;
}

LRESULT CALLBACK gearoenix::nufrag::system::Application::process_callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	auto sys_app = reinterpret_cast<Application *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (WM_CREATE == uMsg) {
		auto create_structure = reinterpret_cast<CREATESTRUCT *>(lParam);
		sys_app = reinterpret_cast<Application *>(create_structure->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(sys_app));
	}
	if (nullptr == sys_app) {
		LOGE(std::string("Unexpected message for nullptr sys app uMsg is: ") + std::to_string(uMsg));
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return sys_app->handle_message(hWnd, uMsg, wParam, lParam);
}

LRESULT gearoenix::nufrag::system::Application::handle_message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		/// TODO: proper termination
		running = false;
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(hWnd, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
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
		//if (camera.firstperson) {
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
		//mousePos.x = (float)LOWORD(lParam);
		//mousePos.y = (float)HIWORD(lParam);
		break;
	case WM_MOUSEWHEEL:
	{
		//short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//zoom += (float)wheelDelta * 0.005f * zoomSpeed;
		//camera.translate(glm::vec3(0.0f, 0.0f, (float)wheelDelta * 0.005f * zoomSpeed));
		//viewUpdated = true;
		break;
	}
	case WM_MOUSEMOVE:
		if (wParam & MK_RBUTTON) {
			//int32_t posx = LOWORD(lParam);
			//int32_t posy = HIWORD(lParam);
			//zoom += (mousePos.y - (float)posy) * .005f * zoomSpeed;
			//camera.translate(glm::vec3(-0.0f, 0.0f, (mousePos.y - (float)posy) * .005f * zoomSpeed));
			//mousePos = glm::vec2((float)posx, (float)posy);
			//viewUpdated = true;
		}
		if (wParam & MK_LBUTTON) {
			//int32_t posx = LOWORD(lParam);
			//int32_t posy = HIWORD(lParam);
			//rotation.x += (mousePos.y - (float)posy) * 1.25f * rotationSpeed;
			//rotation.y -= (mousePos.x - (float)posx) * 1.25f * rotationSpeed;
			//camera.rotate(glm::vec3((mousePos.y - (float)posy) * camera.rotationSpeed, -(mousePos.x - (float)posx) * camera.rotationSpeed, 0.0f));
			//mousePos = glm::vec2((float)posx, (float)posy);
			//viewUpdated = true;
		}
		if (wParam & MK_MBUTTON) {
			//int32_t posx = LOWORD(lParam);
			//int32_t posy = HIWORD(lParam);
			//cameraPos.x -= (mousePos.x - (float)posx) * 0.01f;
			//cameraPos.y -= (mousePos.y - (float)posy) * 0.01f;
			//camera.translate(glm::vec3(-(mousePos.x - (float)posx) * 0.01f, -(mousePos.y - (float)posy) * 0.01f, 0.0f));
			//viewUpdated = true;
			//mousePos.x = (float)posx;
			//mousePos.y = (float)posy;
		}
		break;
	case WM_SIZE:
		//if ((prepared) && (wParam != SIZE_MINIMIZED))
		//{
		//	if ((resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED)))
		//	{
		//		destWidth = LOWORD(lParam);
		//		destHeight = HIWORD(lParam);
		//		windowResize();
		//	}
		//}
		break;
	case WM_SHOWWINDOW:
		if (nullptr == core_app) {
			render_engine = std::shared_ptr<render::Engine>(new render::Engine(this));
			core_app = std::shared_ptr<core::Application>(new core::Application(this));
		}
		break;
	case WM_ENTERSIZEMOVE:
		//resizing = true;
		break;
	case WM_EXITSIZEMOVE:
		//resizing = false;
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

gearoenix::nufrag::system::Application::Application(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     pCmdLine,
	int       nCmdShow) :
	instance(hInstance) {
	setz(wnd_class);
	wnd_class.cbSize = sizeof(WNDCLASSEX);
	wnd_class.style = CS_HREDRAW | CS_VREDRAW;
	wnd_class.lpfnWndProc = process_callback;
	wnd_class.hInstance = instance;
	wnd_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd_class.lpszClassName = TEXT(APPLICATION_NAME);
	wnd_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	if (!RegisterClassEx(&wnd_class)) {
		LOGF("Could not register window class!");
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	DWORD dwExStyle = 0;
	DWORD dwStyle = 0;
	RECT window_rect;
	setz(window_rect);
	window_rect.left = 0L;
	window_rect.top = 0L;
#ifndef WIN_DEBUG
	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth = screenWidth;
	dmScreenSettings.dmPelsHeight = screenHeight;
	dmScreenSettings.dmBitsPerPel = 32;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if ((width != screenWidth) && (height != screenHeight)) {
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
				fullscreen = FALSE;
			} else {
				return FALSE;
			}
		}
	}
	dwExStyle = WS_EX_APPWINDOW;
	dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	windowRect.right = (long)screenWidth;
	windowRect.bottom = (long)screenHeight;
#else
	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	window_rect.right = DEFAULT_WINDOW_WIDTH;
	window_rect.bottom = DEFAULT_WINDOW_HEIGHT;
#endif
	AdjustWindowRectEx(&window_rect, dwStyle, FALSE, dwExStyle);
	window = CreateWindowEx(0,
		TEXT(APPLICATION_NAME),
		TEXT(APPLICATION_NAME),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		NULL,
		NULL,
		instance,
		this);
#ifndef WIN_DEBUG
	uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	SetWindowPos(window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
#endif
	if (!window) {
		LOGF("Could not create window!");
	}
	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(window);
	UpdateWindow(window);
}

gearoenix::nufrag::system::Application::~Application() {}
const HWND & gearoenix::nufrag::system::Application::get_hwnd() const {
	return window;
}
#endif 