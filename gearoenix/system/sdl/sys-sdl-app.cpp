#include "sys-sdl-app.hpp"
#ifdef USE_SDL
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../gles2/gles2-engine.hpp"
#include "../../gles2/gles2.hpp"
#include "../sys-log.hpp"

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
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOGF("Failed to initialize SDL: " << SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    window = SDL_CreateWindow(
        APPLICATION_NAME,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (!window) {
        LOGF("Couldn't create window: " << SDL_GetError());
    }
    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        LOGF("Couldn't create context: " << SDL_GetError());
    }
    SDL_AddEventWatch(event_receiver, this);
    SDL_GL_MakeCurrent(window, gl_context);
    render_engine = new gles2::Engine(this);
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
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        //        SDL_GL_MakeCurrent(window, gl_context);
        render_engine->update();
        SDL_GL_SwapWindow(window);
    }
    core_app->terminate();
    render_engine->terminate();
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
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return static_cast<core::Real>(w) / static_cast<core::Real>(h);
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
