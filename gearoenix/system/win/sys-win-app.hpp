#ifndef GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP
#define GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_WINAPI
#include <Windows.h>
#include "../../core/cr-types.hpp"
namespace gearoenix {
	namespace core {
		namespace asset {
			class Manager;
		}
		class Application;
	}
	namespace render {
		class Engine;
	}
	namespace system {
		class Application {
		private:
			int screen_width, screen_height;
			HINSTANCE instance;
			HWND window;
			bool running = true;
			static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
			LRESULT CALLBACK handler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
		public:
			Application();
			~Application();
			void execute(core::Application* core_app);
			render::Engine* get_render_engine();
			const render::Engine* get_render_engine() const;
			core::asset::Manager* get_asset_manager();
			const core::asset::Manager* get_asset_manager() const;
			core::Real get_window_ratio() const;
		};
	}
}
#endif // USE_WINAPI
#endif // GEAROENIX_SYSTEM_WINDOWS_APPLICATION_HPP