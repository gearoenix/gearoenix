#include "sys-sdl-app.hpp"
#ifdef GX_USE_SDL

#if !defined(GX_USE_VULKAN) && !defined(GX_USE_OPENGL_43) && !defined(GX_USE_OPENGL_33) && !defined(GX_USE_OPENGL_ES3) && !defined(GX_USE_OPENGL_ES2)
#error "Only Vulkan or OpenGL APIs can be used along side of SDL2 application."
#endif

#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/event/cr-ev-bt-keyboard.hpp"
#include "../../core/event/cr-ev-bt-mouse.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-mv-mouse.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../gles2/gles2-engine.hpp"
#include "../../gles2/gles2.hpp"
#include "../../gles3/gles3-engine.hpp"
#include "../../gles3/gles3.hpp"
#include "../../vulkan/vk-engine.hpp"
#include "../sys-log.hpp"
#include <iostream>

#ifdef IN_WEB
gearoenix::system::Application* gearoenix::system::Application::app = nullptr;
#endif

const gearoenix::core::Real gearoenix::system::Application::ROTATION_EPSILON = 3.14f / 180.0f;
const gearoenix::core::Real gearoenix::system::Application::ZOOM_EPSILON = 0.00001f;

void gearoenix::system::Application::create_window()
{
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandescapeRight");
	std::uint32_t flags = SDL_WINDOW_SHOWN;

#ifdef GX_USE_VULKAN
	if (render::EngineType::VULKAN == supported_engine)
	{
		flags |= SDL_WINDOW_VULKAN;
	}
#endif

#if defined(GX_USE_OPENGL_43) || defined(GX_USE_OPENGL_33) || defined(GX_USE_OPENGL_ES3) || defined(GX_USE_OPENGL_ES2)
	if (render::EngineType::VULKAN != supported_engine) {
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		flags |= SDL_WINDOW_OPENGL;
	}
#endif

#ifdef GX_FULLSCREEN
	flags |= SDL_WINDOW_FULLSCREEN;
	flags |= SDL_WINDOW_BORDERLESS;
	flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#else
	flags |= SDL_WINDOW_RESIZABLE;
#endif

#if defined(GX_IN_MAC) || defined(GX_IN_IOS)
	flags |= SDL_WINDOW_ALLOW_HIGHDPI;
#endif

	window = SDL_CreateWindow(
		GX_APP_NAME,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		win_width,
		win_height,
		flags);
	if (nullptr != window) {
		GXLOGI("Best window created.");
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	window = SDL_CreateWindow(
		GX_APP_NAME,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		win_width,
		win_height,
		flags);
	if (nullptr != window) {
		GXLOGI("Window with disabled multisamples created.");
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	window = SDL_CreateWindow(
		GX_APP_NAME,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		win_width,
		win_height,
		flags);
	if (nullptr != window) {
		GXLOGI("Window with minimum rquirement created.");
		return;
	}
	GXLOGF("Can not create window with minimum requirements");
}

void gearoenix::system::Application::create_context()
{
#ifdef GX_USE_VULKAN
	if (render::EngineType::VULKAN == supported_engine) {
		return;
	}
#endif

#ifdef GX_USE_OPENGL_43
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	gl_context = SDL_GL_CreateContext(window);
	if (gl_context != nullptr) {
		supported_engine = render::EngineType::OPENGL_43;
		glEnable(GL_MULTISAMPLE);
		GXLOGD("Machine is capable if OpenGL 4.3");
		return;
	}
#endif

#ifdef GX_USE_OPENGL_33
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	gl_context = SDL_GL_CreateContext(window);
	if (gl_context != nullptr) {
		supported_engine = render::EngineType::OPENGL_33;
		glEnable(GL_MULTISAMPLE);
		GXLOGD("Machine is capable if OpenGL 3.3");
		return;
	}
#endif

#ifdef GX_USE_OPENGL_ES3
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	gl_context = SDL_GL_CreateContext(window);
	if (gl_context != nullptr) {
		supported_engine = render::EngineType::OPENGL_ES3;
		glEnable(GL_MULTISAMPLE);
		GXLOGD("Machine is capable if OpenGL ES 3.0");
		return;
	}
#endif

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	gl_context = SDL_GL_CreateContext(window);
	if (gl_context != nullptr) {
		supported_engine = render::EngineType::OPENGL_ES2;
		GXLOGD("Machine is capable if OpenGL ES 2.0");
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	gl_context = SDL_GL_CreateContext(window);
	if (gl_context != nullptr) {
		supported_engine = render::EngineType::OPENGL_ES2;
		GXLOGD("Machine is capable if weak OpenGL ES 2.0");
		return;
	}
	GXLOGF("No usable API find in the host machine.");
}

int SDLCALL gearoenix::system::Application::event_receiver(void* user_data, SDL_Event* e)
{
	// It's gonna implement whenever needed and as much as needed.
	Application* o = static_cast<Application*>(user_data);
	core::event::Event* event = nullptr;
	switch (e->type) {
	case SDL_APP_WILLENTERBACKGROUND:
		o->running = false;
		break;
	case SDL_KEYDOWN:
		switch (e->key.keysym.sym) {
		case SDLK_LEFT:
			event = new core::event::button::Keyboard(core::event::button::Keyboard::LEFT, core::event::button::Keyboard::PRESS);
			break;
		case SDLK_RIGHT:
			event = new core::event::button::Keyboard(core::event::button::Keyboard::RIGHT, core::event::button::Keyboard::PRESS);
			break;
		case SDLK_UP:
			event = new core::event::button::Keyboard(core::event::button::Keyboard::UP, core::event::button::Keyboard::PRESS);
			break;
		case SDLK_ESCAPE:
			o->running = false;
			break;
		default:
			break;
		}
		break;

#ifdef GX_IN_IOS
	case SDL_FINGERDOWN: {
		const core::Real x = (e->tfinger.x - 0.5f) * 2.0f * o->screen_ratio;
		const core::Real y = (0.5f - e->tfinger.y) * 2.0f;
		event = new core::event::button::Mouse(
			core::event::button::Button::KeyType::LEFT,
			core::event::button::Button::ActionType::PRESS,
			x, y);
		o->pre_x = x;
		o->pre_y = y;
		break;
	}
	case SDL_FINGERUP: {
		const core::Real x = (e->tfinger.x - 0.5f) * 2.0f * o->screen_ratio;
		const core::Real y = (0.5f - e->tfinger.y) * 2.0f;
		event = new core::event::button::Mouse(
			core::event::button::Button::KeyType::LEFT,
			core::event::button::Button::ActionType::RELEASE,
			x, y);
		o->pre_x = x;
		o->pre_y = y;
		break;
	}
	case SDL_FINGERMOTION: {
		const core::Real x = (e->tfinger.x - 0.5f) * 2.0f * o->screen_ratio;
		const core::Real y = (0.5f - e->tfinger.y) * 2.0f;
		event = new core::event::movement::Mouse(x, y, o->pre_x, o->pre_y);
		o->pre_x = x;
		o->pre_y = y;
		break;
	}
#endif

	case SDL_MOUSEWHEEL:
		// todo
		// o->core_app->on_scroll((core::Real)event->wheel.y);
		break;
	case SDL_MOUSEMOTION: {
		const core::Real x = o->convert_x_to_ratio(e->button.x);
		const core::Real y = o->convert_y_to_ratio(e->button.y);
		event = new core::event::movement::Mouse(x, y, o->pre_x, o->pre_y);
		o->pre_x = x;
		o->pre_y = y;
		break;
	}
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN: {
		core::event::button::Button::ActionType a = core::event::button::Button::ActionType::PRESS;
		switch (e->type) {
		case SDL_MOUSEBUTTONUP:
			a = core::event::button::Button::ActionType::RELEASE;
			break;
		default:
			break;
		}
		core::event::button::Button::KeyType k = core::event::button::Button::KeyType::LEFT;
		switch (e->button.button) {
		case SDL_BUTTON_RIGHT:
			k = core::event::button::Button::KeyType::RIGHT;
			break;
		case SDL_BUTTON_MIDDLE:
			k = core::event::button::Button::KeyType::MIDDLE;
			break;
		default:
			break;
		}
		const core::Real x = o->convert_x_to_ratio(e->button.x);
		const core::Real y = o->convert_y_to_ratio(e->button.y);
		event = new core::event::button::Mouse(k, a, x, y);
		break;
	}
	case SDL_MULTIGESTURE:
		if (e->mgesture.dTheta > ROTATION_EPSILON || e->mgesture.dTheta < -ROTATION_EPSILON) {
			// o->core_app->on_rotate(event->mgesture.dTheta);
		}
		else if (e->mgesture.dDist > ZOOM_EPSILON || e->mgesture.dDist < -ZOOM_EPSILON) {
			// o->core_app->on_zoom(event->mgesture.dDist);
		}
		break;
	case SDL_WINDOWEVENT:
		switch (e->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			event = new core::event::WindowResize(
				(core::Real) o->win_width,
				(core::Real) o->win_height,
				(core::Real) e->window.data1,
				(core::Real) e->window.data2);
			o->win_width = e->window.data1;
			o->win_height = e->window.data2;
			o->screen_ratio = (core::Real)o->win_width / (core::Real)o->win_height;
			o->half_height_inversed = 2.0f / (core::Real)o->win_height;
			break;
		default:
			GXTODO;
			break;
		}
		break;
	default:
		GXLOGE("Unhandled event " << e->type);
		break;
	}
	if (event != nullptr) {
		o->render_engine->on_event(*event);
		o->core_app->on_event(*event);
		delete event;
	}
	return 1;
}

gearoenix::system::Application::Application()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		GXLOGF("Failed to initialize SDL: " << SDL_GetError());
	}

#ifdef GX_USE_VULKAN
	if (vulkan::Engine::is_supported())
	{
		supported_engine = render::EngineType::VULKAN;
	}
#endif

#ifdef GX_FULLSCREEN
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	win_width = display_mode.w;
	win_height = display_mode.h;
#else
	win_width = GEAROENIX_DEFAULT_WINDOW_WIDTH;
	win_height = GEAROENIX_DEFAULT_WINDOW_HEIGHT;
#endif

	create_window();
	create_context();

	SDL_AddEventWatch(event_receiver, this);

	if (supported_engine == render::EngineType::OPENGL_43 ||
		supported_engine == render::EngineType::OPENGL_33 ||
		supported_engine == render::EngineType::OPENGL_ES3 ||
		supported_engine == render::EngineType::OPENGL_ES3) {

#if defined(GX_IN_DESKTOP) && (defined(GX_USE_OPENGL_ES2) || defined(GX_USE_OPENGL_ES3) || defined(GX_USE_OPENGL_33) || defined(GX_USE_OPENGL_43))
		const GLenum glew_error = glewInit();
		if (glew_error != GLEW_OK) {
			GXLOGF("Error initializing GLEW! " << glewGetErrorString(glew_error));
		}
#endif

		SDL_GL_MakeCurrent(window, gl_context);
		int w, h;
		SDL_GL_GetDrawableSize(window, &w, &h);
		win_width = (unsigned int)w;
		win_height = (unsigned int)h;
	}
	screen_ratio = (core::Real)win_width / (core::Real)win_height;
	half_height_inversed = 2.0f / (core::Real)win_height;
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	pre_x = convert_x_to_ratio(mx);
    pre_y = convert_y_to_ratio(my);

#ifdef GX_USE_VULKAN
	if (nullptr == render_engine && supported_engine == render::EngineType::VULKAN)
	{
		render_engine = new vulkan::Engine(this);
	}
#endif

#ifdef GX_USE_OPENGL_43
	if (nullptr == render_engine && supported_engine == render::Engine::OPENGL_43) 
	{
		render_engine = new gl43::Engine(this);
	}
#endif

#ifdef GX_USE_OPENGL_33
	if (nullptr == render_engine && supported_engine == render::Engine::OPENGL_33) 
	{
		render_engine = new gl33::Engine(this);
	}
#endif

#ifdef GX_USE_OPENGL_ES3
	if (nullptr == render_engine && supported_engine == render::Engine::OPENGL_ES3) 
	{
		render_engine = new gles3::Engine(this);
	}
#endif

#ifdef GX_USE_OPENGL_ES2
	if (nullptr == render_engine && supported_engine == render::EngineType::OPENGL_ES2)
	{
		render_engine = new gles2::Engine(this);
	}
#endif

	if (render_engine == nullptr)
	{
		GXLOGF("No suitable render engine found.");
    }

    astmgr = new core::asset::Manager(this, "data.gx3d");
}

gearoenix::system::Application::~Application()
{
	delete core_app;
	delete astmgr;
	delete render_engine;
}

void gearoenix::system::Application::execute(core::Application* app)
{
	core_app = app;
#ifdef GX_IN_WEB
	Application::app = this;
	emscripten_set_main_loop(Application::loop, 0, true);
}

void gearoenix::system::Application::loop()
{
	app->main_loop();
}

void gearoenix::system::Application::main_loop()
{
#else
	while (running) {
#endif
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}
		// SDL_GL_MakeCurrent(window, gl_context);
		render_engine->update();

		if (supported_engine == render::EngineType::OPENGL_43 ||
			supported_engine == render::EngineType::OPENGL_33 ||
			supported_engine == render::EngineType::OPENGL_ES3 ||
			supported_engine == render::EngineType::OPENGL_ES3) {
			SDL_GL_SwapWindow(window);
		}
#ifndef GX_IN_WEB
	}
	core_app->terminate();
	render_engine->terminate();
	SDL_DelEventWatch(event_receiver, this);
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
#endif
}

const gearoenix::core::Application* gearoenix::system::Application::get_core_app() const
{
	return core_app;
}

gearoenix::core::Application* gearoenix::system::Application::get_core_app()
{
	return core_app;
}

const gearoenix::render::Engine* gearoenix::system::Application::get_render_engine() const
{
	return render_engine;
}

gearoenix::render::Engine* gearoenix::system::Application::get_render_engine()
{
	return render_engine;
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
	return screen_ratio;
}

unsigned int gearoenix::system::Application::get_width() const
{
	return win_width;
}

unsigned int gearoenix::system::Application::get_height() const
{
	return win_height;
}

gearoenix::core::Id gearoenix::system::Application::get_supported_engine() const
{
	return supported_engine;
}

gearoenix::core::Real gearoenix::system::Application::convert_x_to_ratio(int x) const
{
	return (((core::Real)x) * half_height_inversed) - screen_ratio;
}

gearoenix::core::Real gearoenix::system::Application::convert_y_to_ratio(int y) const
{
	return 1.0f - (((core::Real)y) * half_height_inversed);
}

#endif
