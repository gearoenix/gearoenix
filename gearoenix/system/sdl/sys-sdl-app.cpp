#include "sys-sdl-app.hpp"
#ifdef GX_USE_SDL

#if !defined(GX_USE_VULKAN) && !defined(GX_USE_OPENGL)
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
#include "../../gl/gl-loader.hpp"
#if defined(GX_USE_OPENGL_ES2)
#include "../../gles2/engine/gles2-eng-engine.hpp"
#endif
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../glc3/engine/glc3-eng-engine.hpp"
#endif
#if defined(GX_USE_VULKAN)
#include "../../vulkan/engine/vk-eng-engine.hpp"
#endif
#include "../sys-configuration.hpp"
#include "../sys-log.hpp"
#include <iostream>

#ifdef GX_IN_WEB
gearoenix::system::Application* gearoenix::system::Application::app = nullptr;
#endif

const gearoenix::core::Real gearoenix::system::Application::ROTATION_EPSILON = 3.14f / 180.0f;
const gearoenix::core::Real gearoenix::system::Application::ZOOM_EPSILON = 0.00001f;

void gearoenix::system::Application::create_window() noexcept
{
#ifndef GX_IN_DESKTOP
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "Landscape");
#endif
    std::uint32_t flags = SDL_WINDOW_SHOWN;
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

#ifdef GX_USE_VULKAN
    if (render::engine::Type::VULKAN == supported_engine) {
        flags |= SDL_WINDOW_VULKAN;
        window = SDL_CreateWindow(
            GX_APP_NAME,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            static_cast<int>(win_width),
            static_cast<int>(win_height),
            flags);
        if (nullptr != window) {
            GXLOGI("Vulkan SDL2 window created.")
            return;
        }
    }
#endif
#if defined(GX_USE_OPENGL)
    SDL_GL_SetSwapInterval(0);
    flags |= SDL_WINDOW_OPENGL;
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#endif
#define CREATE_WINDOW(gl_version)                                   \
    supported_engine = render::engine::Type::gl_version;            \
    window = SDL_CreateWindow(                                      \
        GX_APP_NAME,                                                \
        SDL_WINDOWPOS_CENTERED,                                     \
        SDL_WINDOWPOS_CENTERED,                                     \
        static_cast<int>(win_width),                                \
        static_cast<int>(win_height),                               \
        flags);                                                     \
    if (nullptr != window) {                                        \
        gl_context = SDL_GL_CreateContext(window);                  \
        if (gl_context != nullptr) {                                \
            if (gl::Loader::load_library(supported_engine)) {       \
                GXLOGD("OpenGL window built with: " << #gl_version) \
                return;                                             \
            }                                                       \
            SDL_GL_DeleteContext(gl_context);                       \
        }                                                           \
        SDL_DestroyWindow(window);                                  \
    }

#ifdef GX_USE_OPENGL_43
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    CREATE_WINDOW(OPENGL_43)
#endif
#ifdef GX_USE_OPENGL_33
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    CREATE_WINDOW(OPENGL_33)
#endif
#ifdef GX_USE_OPENGL_ES3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    CREATE_WINDOW(OPENGL_ES3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    CREATE_WINDOW(OPENGL_ES3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    CREATE_WINDOW(OPENGL_ES3)
#endif
#ifdef GX_USE_OPENGL_ES2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    CREATE_WINDOW(OPENGL_ES2)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    CREATE_WINDOW(OPENGL_ES2)
#endif
    GXLOGF("Can not create window with minimum requirements")
}

int SDLCALL gearoenix::system::Application::event_receiver(void* user_data, SDL_Event* e) noexcept
{
    // It's gonna implement whenever needed and as much as needed.
    auto* o = static_cast<Application*>(user_data);
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
        } else if (e->mgesture.dDist > ZOOM_EPSILON || e->mgesture.dDist < -ZOOM_EPSILON) {
            // o->core_app->on_zoom(event->mgesture.dDist);
        }
        break;
    case SDL_WINDOWEVENT:
        switch (e->window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            event = new core::event::WindowResize(
                static_cast<core::Real>(o->win_width),
                static_cast<core::Real>(o->win_height),
                static_cast<core::Real>(e->window.data1),
                static_cast<core::Real>(e->window.data2));
            o->win_width = static_cast<unsigned int>(e->window.data1);
            o->win_height = static_cast<unsigned int>(e->window.data2);
            o->screen_ratio = static_cast<core::Real>(o->win_width) / static_cast<core::Real>(o->win_height);
            o->half_height_inversed = 2.0f / static_cast<core::Real>(o->win_height);
            break;
        default:
            GXTODO
            break;
        }
        break;
    default:
        GXLOGE("Unhandled event " << e->type)
        break;
    }
    if (event != nullptr) {
        // TODO: new event system must be implemented
        GXTODO
        // o->render_engine->on_event(*event);
        //o->core_app->on_event(*event);
        delete event;
    }
    return 1;
}

const std::shared_ptr<gearoenix::system::Application> gearoenix::system::Application::construct() noexcept
{
    const std::shared_ptr<Application> result(new Application());
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0) {
        GXLOGF("Failed to initialize SDL: " << SDL_GetError())
    }
#ifdef GX_USE_VULKAN
    if (vulkan::Engine::is_supported()) {
        result->supported_engine = render::engine::Type::VULKAN;
    }
#endif

#ifdef GX_FULLSCREEN
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    result->win_width = static_cast<unsigned int>(display_mode.w);
    result->win_height = static_cast<unsigned int>(display_mode.h);
#else
    result->win_width = GX_DEFAULT_WINDOW_WIDTH;
    result->win_height = GX_DEFAULT_WINDOW_HEIGHT;
#endif

    result->create_window();

    SDL_AddEventWatch(event_receiver, result.get());
#ifdef GX_USE_OPENGL
    if (GX_RUNTIME_USE_OPENGL_V(result->supported_engine)) {
        int w, h;
        SDL_GL_GetDrawableSize(result->window, &w, &h);
        result->win_width = static_cast<unsigned int>(w);
        result->win_height = static_cast<unsigned int>(h);
    }
#endif
    result->screen_ratio = static_cast<core::Real>(result->win_width) / static_cast<core::Real>(result->win_height);
    result->half_height_inversed = 2.0f / static_cast<core::Real>(result->win_height);
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    result->pre_x = result->convert_x_to_ratio(mx);
    result->pre_y = result->convert_y_to_ratio(my);

    result->config = std::make_shared<Configuration>();
#ifdef GX_USE_VULKAN
    if (nullptr == result->render_engine && result->supported_engine == render::engine::Type::VULKAN) {
        result->render_engine = new vulkan::Engine(this);
    }
#endif

#ifdef GX_USE_OPENGL_CLASS_3
    if (nullptr == result->render_engine && (GX_RUNTIME_USE_OPENGL_CLASS_3_V(result->supported_engine))) {
        result->render_engine = glc3::engine::Engine::construct(result.get(), result->supported_engine);
    }
#endif

#ifdef GX_USE_OPENGL_ES2
    if (nullptr == result->render_engine && result->supported_engine == render::engine::Type::OPENGL_ES2) {
        result->render_engine = gles2::engine::Engine::construct(result.get());
    }
#endif

    if (result->render_engine == nullptr) {
        GXLOGF("No suitable render engine found.")
    }

    result->astmgr = std::make_shared<core::asset::Manager>(result, GX_APP_DATA_NAME);
    return result;
}

gearoenix::system::Application::~Application() noexcept
{
    core_app = nullptr;
    astmgr = nullptr;
    render_engine = nullptr;
}

void gearoenix::system::Application::execute(const std::shared_ptr<core::Application>& app) noexcept
{
    core_app = app;
#ifdef GX_IN_WEB
    Application::app = this;
    emscripten_set_main_loop(Application::loop, 0, true);
}

void gearoenix::system::Application::loop() noexcept
{
    app->main_loop();
}

void gearoenix::system::Application::main_loop() noexcept
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
    core_app->update();
    render_engine->update();
#ifdef GX_USE_OPENGL
    if (GX_RUNTIME_USE_OPENGL_V(supported_engine)) {
        SDL_GL_SwapWindow(window);
    }
#endif
#ifndef GX_IN_WEB
}
core_app->terminate();
core_app = nullptr;
render_engine = nullptr;
astmgr = nullptr;
SDL_DelEventWatch(event_receiver, this);
#ifdef GX_USE_OPENGL
#ifdef GX_USE_INSTEAD_OF_OPENGL
if (GX_RUNTIME_USE_OPENGL_V(supported_engine)) {
#endif
    SDL_GL_DeleteContext(gl_context);
    gl::Loader::unload_library();
#ifdef GX_USE_INSTEAD_OF_OPENGL
}
#endif
#endif
SDL_DestroyWindow(window);
SDL_Quit();
#endif
}

const std::shared_ptr<gearoenix::core::Application>& gearoenix::system::Application::get_core_app() const noexcept
{
    return core_app;
}

std::shared_ptr<gearoenix::core::Application>& gearoenix::system::Application::get_core_app() noexcept
{
    return core_app;
}

const std::shared_ptr<gearoenix::render::engine::Engine>& gearoenix::system::Application::get_render_engine() const noexcept
{
    return render_engine;
}

std::shared_ptr<gearoenix::render::engine::Engine>& gearoenix::system::Application::get_render_engine() noexcept
{
    return render_engine;
}

const std::shared_ptr<gearoenix::core::asset::Manager>& gearoenix::system::Application::get_asset_manager() const noexcept
{
    return astmgr;
}

std::shared_ptr<gearoenix::core::asset::Manager>& gearoenix::system::Application::get_asset_manager() noexcept
{
    return astmgr;
}

const gearoenix::system::Configuration& gearoenix::system::Application::get_configuration() const noexcept
{
    return *config;
}

gearoenix::system::Configuration& gearoenix::system::Application::get_configuration() noexcept
{
    return *config;
}

gearoenix::core::Real gearoenix::system::Application::get_window_ratio() const noexcept
{
    return screen_ratio;
}

unsigned int gearoenix::system::Application::get_width() const noexcept
{
    return win_width;
}

unsigned int gearoenix::system::Application::get_height() const noexcept
{
    return win_height;
}

gearoenix::core::Real gearoenix::system::Application::convert_x_to_ratio(int x) const noexcept
{
    return (static_cast<core::Real>(x) * half_height_inversed) - screen_ratio;
}

gearoenix::core::Real gearoenix::system::Application::convert_y_to_ratio(int y) const noexcept
{
    return 1.0f - (static_cast<core::Real>(y) * half_height_inversed);
}

#endif
