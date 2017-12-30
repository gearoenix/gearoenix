#include "sys-sdl-app.hpp"
#ifdef USE_SDL
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
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

void gearoenix::system::Application::create_context()
{
#ifdef IN_DESKTOP
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context != nullptr) {
        supported_engine = render::Engine::EngineType::OPENGL_43;
        glEnable(GL_MULTISAMPLE);
        GXLOGD("Machine is capable if OpenGL 4.3");
        return;
    }
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
        o->core_app->on_scroll((core::Real)event->wheel.y);
        break;
    case SDL_MOUSEMOTION:
        o->core_app->on_mouse_move((core::Real)event->motion.xrel, (core::Real)event->motion.yrel);
        break;
    case SDL_MOUSEBUTTONDOWN:
        switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            o->core_app->on_mouse(core::Application::MouseButton::LEFT, core::Application::ButtonAction::PRESS, (core::Real)event->button.x, (core::Real)event->button.y);
            break;
        case SDL_BUTTON_RIGHT:
            o->core_app->on_mouse(core::Application::MouseButton::RIGHT, core::Application::ButtonAction::PRESS, (core::Real)event->button.x, (core::Real)event->button.y);
            break;
        case SDL_BUTTON_MIDDLE:
            o->core_app->on_mouse(core::Application::MouseButton::MIDDLE, core::Application::ButtonAction::PRESS, (core::Real)event->button.x, (core::Real)event->button.y);
            break;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            o->core_app->on_mouse(core::Application::MouseButton::LEFT, core::Application::ButtonAction::RELEASE, (core::Real)event->button.x, (core::Real)event->button.y);
            break;
        case SDL_BUTTON_RIGHT:
            o->core_app->on_mouse(core::Application::MouseButton::RIGHT, core::Application::ButtonAction::RELEASE, (core::Real)event->button.x, (core::Real)event->button.y);
            break;
        case SDL_BUTTON_MIDDLE:
            o->core_app->on_mouse(core::Application::MouseButton::MIDDLE, core::Application::ButtonAction::RELEASE, (core::Real)event->button.x, (core::Real)event->button.y);
            break;
        }
        break;
    case SDL_MULTIGESTURE:
        if (event->mgesture.dTheta > rotate_epsilon || event->mgesture.dTheta < -rotate_epsilon) {
            o->core_app->on_rotate(event->mgesture.dTheta);
        } else if (event->mgesture.dDist > zoom_epsilon || event->mgesture.dDist < -zoom_epsilon) {
            o->core_app->on_zoom(event->mgesture.dDist);
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
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        GXLOGF("Failed to initialize SDL: " << SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandescapeRight");
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window = SDL_CreateWindow(
        APPLICATION_NAME,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        display_mode.w,
        display_mode.h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN |
        SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP |
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        GXLOGF("Couldn't create window: " << SDL_GetError());
    }
    create_context();
    
    SDL_AddEventWatch(event_receiver, this);
    SDL_GL_MakeCurrent(window, gl_context);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GL_GetDrawableSize(window, &w, &h);
    win_width = (unsigned int)w;
    win_height = (unsigned int)h;
#ifdef USE_OPENGL_ES2
    if(supported_engine == render::Engine::OPENGL_ES2) render_engine = new gles2::Engine(this);
#else
    if(supported_engine != render::Engine::OPENGL_ES2) render_engine = new gles3::Engine(this);
#endif
#ifdef DEBUG_MODE
    if(render_engine == nullptr) UNEXPECTED;
#endif
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
    return static_cast<core::Real>(win_width) / static_cast<core::Real>(win_height);
}

unsigned int gearoenix::system::Application::get_width() const
{
    return win_width;
}

unsigned int gearoenix::system::Application::get_height() const
{
    return win_height;
}

#endif
