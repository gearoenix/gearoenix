#ifndef GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP
#define GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_WINAPI
#include <Windows.h>
namespace gearoenix {
	namespace core {
		class Application;
	}
	namespace system {
		class Application {
		private:
			int screen_width, screen_height;
			HINSTANCE instance;
			HWND window;
			static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
			LRESULT CALLBACK handler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
		public:
			Application();
			~Application();
			void execute(core::Application* core_app);
		};
	}
}
#endif // USE_WINAPI
#endif // GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP