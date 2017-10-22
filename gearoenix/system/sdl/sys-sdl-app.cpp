#include "sys-sdl-app.hpp"
#ifdef USE_SDL
#include "../../gles2/gles2-engine.hpp"
#include "../sys-log.hpp"
#include <SDL2/SDL_opengles2.h>

int SDLCALL gearoenix::system::Application::event_receiver(void* user_data, SDL_Event* event)
{
    Application* o = reinterpret_cast<Application*>(user_data);
    if (event->type == SDL_APP_WILLENTERBACKGROUND) {
        o->running = false;
    }
    return 1;
}

gearoenix::system::Application::Application()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LOGF("Failed to initialize SDL: " << SDL_GetError());
    }
    window = SDL_CreateWindow(
        APPLICATION_NAME,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);
    gl_context = SDL_GL_CreateContext(window);
    SDL_AddEventWatch(event_receiver, this);
    SDL_GL_MakeCurrent(window, gl_context);
    render_engine = new gles2::Engine(this);
}

gearoenix::system::Application::~Application()
{
    TODO;
}

void gearoenix::system::Application::execute(core::Application* app)
{
    core_app = app;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        render_engine->update();
        SDL_GL_SwapWindow(window);
    }
    SDL_DelEventWatch(event_receiver, this);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
    TODO;
    return 1.7f;
}

unsigned int gearoenix::system::Application::get_width() const
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    (void)h;
    return static_cast<unsigned int>(w);
}

unsigned int gearoenix::system::Application::get_height() const
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    (void)w;
    return static_cast<unsigned int>(h);
}

#endif
