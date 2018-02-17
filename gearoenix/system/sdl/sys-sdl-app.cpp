#include "sys-sdl-app.hpp"
#ifdef USE_SDL
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/event/cr-ev-bt-mouse.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-mv-mouse.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../gles2/gles2-engine.hpp"
#include "../../gles2/gles2.hpp"
#include "../../gles3/gles3-engine.hpp"
#include "../../gles3/gles3.hpp"
#include "../sys-log.hpp"
#include <iostream>

#ifdef IN_WEB
gearoenix::system::Application* gearoenix::system::Application::app = nullptr;
#endif
const gearoenix::core::Real gearoenix::system::Application::rotate_epsilon = 3.14f / 180.0f;
const gearoenix::core::Real gearoenix::system::Application::zoom_epsilon = 0.00001f;

void gearoenix::system::Application::create_window()
{
    std::uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
#ifdef GEAROENIX_FULLSCREEN
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP |
#else
        SDL_WINDOW_RESIZABLE |
#endif
#if defined(IN_MAC) || defined(IN_IOS)
        SDL_WINDOW_ALLOW_HIGHDPI |
#endif
        0;
    window = SDL_CreateWindow(
        APPLICATION_NAME,
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
        APPLICATION_NAME,
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
        APPLICATION_NAME,
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
#ifdef USE_OPENGL_43
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context != nullptr) {
        supported_engine = render::Engine::EngineType::OPENGL_43;
        glEnable(GL_MULTISAMPLE);
        GXLOGD("Machine is capable if OpenGL 4.3");
        return;
    }
#endif
#ifdef USE_OPENGL_33
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context != nullptr) {
        supported_engine = render::Engine::EngineType::OPENGL_33;
        glEnable(GL_MULTISAMPLE);
        GXLOGD("Machine is capable if OpenGL 3.3");
        return;
    }
#endif
#ifdef USE_OPENGL_ES3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context != nullptr) {
        supported_engine = render::Engine::EngineType::OPENGL_ES3;
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
        supported_engine = render::Engine::EngineType::OPENGL_ES2;
        GXLOGD("Machine is capable if OpenGL ES 2.0");
        return;
    }
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context != nullptr) {
        supported_engine = render::Engine::EngineType::OPENGL_ES2;
        GXLOGD("Machine is capable if weak OpenGL ES 2.0");
        return;
    }
    GXLOGF("No usable API find in the host machine.");
}

int SDLCALL gearoenix::system::Application::event_receiver(void* user_data, SDL_Event* event)
{
    // It's gonna implement whenever needed and as much as needed.
    Application* o = reinterpret_cast<Application*>(user_data);
    switch (event->type) {
    case SDL_APP_WILLENTERBACKGROUND:
        o->running = false;
        break;
    case SDL_KEYDOWN:
        switch (event->key.keysym.sym) {
        case SDLK_ESCAPE:
            o->running = false;
            break;
        default:
            break;
        }
        break;
    case SDL_MOUSEWHEEL:
        // todo
        // o->core_app->on_scroll((core::Real)event->wheel.y);
        break;
    case SDL_MOUSEMOTION: {
        const core::Real x = o->convert_x_to_ratio(event->button.x);
        const core::Real y = o->convert_y_to_ratio(event->button.y);
        core::event::movement::Mouse e(x, y, o->pre_x, o->pre_y);
        o->render_engine->on_event(e);
        o->core_app->on_event(e);
        o->pre_x = x;
        o->pre_y = y;
        break;
    }
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN: {
        core::event::button::Button::ActionType a = core::event::button::Button::ActionType::PRESS;
        switch (event->type) {
        case SDL_MOUSEBUTTONUP:
            a = core::event::button::Button::ActionType::RELEASE;
            break;
        default:
            break;
        }
        core::event::button::Button::KeyType k = core::event::button::Button::KeyType::LEFT;
        switch (event->button.button) {
        case SDL_BUTTON_RIGHT:
            k = core::event::button::Button::KeyType::RIGHT;
            break;
        case SDL_BUTTON_MIDDLE:
            k = core::event::button::Button::KeyType::MIDDLE;
            break;
        default:
            break;
        }
        const core::Real x = o->convert_x_to_ratio(event->button.x);
        const core::Real y = o->convert_y_to_ratio(event->button.y);
        core::event::button::Mouse e(k, a, x, y);
        o->render_engine->on_event(e);
        o->core_app->on_event(e);
        break;
    }
    case SDL_MULTIGESTURE:
        if (event->mgesture.dTheta > rotate_epsilon || event->mgesture.dTheta < -rotate_epsilon) {
            // o->core_app->on_rotate(event->mgesture.dTheta);
        } else if (event->mgesture.dDist > zoom_epsilon || event->mgesture.dDist < -zoom_epsilon) {
            // o->core_app->on_zoom(event->mgesture.dDist);
        }
        break;
    case SDL_WINDOWEVENT:
        switch (event->window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
            core::event::WindowResize e(o->win_width, o->win_height, event->window.data1, event->window.data2);
            o->win_width = event->window.data1;
            o->win_height = event->window.data2;
            o->screen_ratio = (core::Real)o->win_width / (core::Real)o->win_height;
            o->half_height_inversed = 2.0f / (core::Real)o->win_height;
            o->render_engine->on_event(e);
            o->core_app->on_event(e);
            break;
        }
        default:
            TODO;
            break;
        }
        break;
    default:
        GXLOGE("Unhandled event " << event->type);
        break;
    }
    return 1;
}

gearoenix::system::Application::Application()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        GXLOGF("Failed to initialize SDL: " << SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
#if defined(USE_OPENGL_43) || defined(USE_OPENGL_33)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#elif defined(USE_OPENGL_ES3) || defined(USE_OPENGL_ES2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
#error "Unexpected"
#endif
#ifdef USE_OPENGL_43
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif defined(USE_OPENGL_33)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif defined(USE_OPENGL_ES3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(USE_OPENGL_ES2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
#error "Unexpected"
#endif
#if defined(USE_OPENGL_43) || defined(USE_OPENGL_33) || defined(USE_OPENGL_ES3)
#ifdef IN_LINUX
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#else
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
#endif
#elif defined(USE_OPENGL_ES2)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#else
#error "Unexpected"
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandescapeRight");
#ifdef GEAROENIX_FULLSCREEN
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
    SDL_GL_MakeCurrent(window, gl_context);
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);
    win_width = (unsigned int)w;
    win_height = (unsigned int)h;
    screen_ratio = (core::Real)win_width / (core::Real)win_height;
    half_height_inversed = 2.0f / (core::Real)win_height;
    SDL_GetMouseState(&w, &h);
    pre_x = convert_x_to_ratio(w);
    pre_y = convert_y_to_ratio(h);
#ifdef USE_OPENGL_ES2
    if (supported_engine == render::Engine::OPENGL_ES2)
        render_engine = new gles2::Engine(this);
#else
    if (supported_engine != render::Engine::OPENGL_ES2)
        render_engine = new gles3::Engine(this);
#endif
    if (render_engine == nullptr)
        UNEXPECTED;
    astmgr = new core::asset::Manager(this, "data.gx3d");
    astmgr->initialize();
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
#ifdef IN_WEB
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
    //        SDL_GL_MakeCurrent(window, gl_context);
    render_engine->update();
    SDL_GL_SwapWindow(window);
#ifndef IN_WEB
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
