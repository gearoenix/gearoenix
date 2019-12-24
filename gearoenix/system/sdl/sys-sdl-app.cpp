#include "sys-sdl-app.hpp"
#ifdef GX_USE_SDL

#if !defined(GX_USE_VULKAN) && !defined(GX_USE_OPENGL)
#error "Only Vulkan or OpenGL APIs can be used along side of SDL2 application."
#endif

#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../gl/gl-loader.hpp"
#include "../sys-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gles2/engine/gles2-eng-engine.hpp"
#endif
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../glc3/engine/glc3-eng-engine.hpp"
#endif
#ifdef GX_USE_VULKAN
#include "../../vulkan/engine/vk-eng-engine.hpp"
#endif
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
#define CREATE_WINDOW(gl_version)                                           \
    supported_engine = render::engine::Type::gl_version;                    \
    GXLOGD("Trying to build OpenGL window with: " << #gl_version)           \
    window = SDL_CreateWindow(                                              \
        GX_APP_NAME,                                                        \
        static_cast<int>(SDL_WINDOWPOS_CENTERED),                           \
        static_cast<int>(SDL_WINDOWPOS_CENTERED),                           \
        static_cast<int>(window_width),                                     \
        static_cast<int>(window_height),                                    \
        flags);                                                             \
    if (nullptr != window) {                                                \
        GXLOGD("Trying to build OpenGL context with: " << #gl_version)      \
        gl_context = SDL_GL_CreateContext(window);                          \
        if (gl_context != nullptr) {                                        \
            GXLOGD("Trying to load OpenGL library with: " << #gl_version)   \
            if (gl::Loader::load_library(supported_engine)) {               \
                GXLOGD("OpenGL window built with: " << #gl_version)         \
                return;                                                     \
            } else                                                          \
                GXLOGD("Can not load OpenGL library with: " << #gl_version) \
            SDL_GL_DeleteContext(gl_context);                               \
        } else                                                              \
            GXLOGD("Can not build OpenGL context with: " << #gl_version)    \
        SDL_DestroyWindow(window);                                          \
    } else {                                                                \
        GXLOGD("Can not build OpenGL window with: " << #gl_version)         \
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
    return reinterpret_cast<Application*>(user_data)->on_event(e);
}

int gearoenix::system::Application::on_event(SDL_Event* const e) noexcept
{
    core::event::Data event = {};
    // It's gonna be implemented whenever needed and as much as needed.
    switch (e->type) {
    case SDL_APP_WILLENTERBACKGROUND:
        running = false;
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
        core::event::button::KeyboardData d;
        d.action = [&]() noexcept {
            switch (e->type) {
            case SDL_KEYUP:
                return core::event::button::KeyboardActionId::Release;
            case SDL_KEYDOWN:
                return core::event::button::KeyboardActionId::Press;
            default:
                return core::event::button::KeyboardActionId::Unknown;
            }
        }();
        d.key = [&]() noexcept {
            switch (e->key.keysym.sym) {
            case SDLK_LEFT:
                return core::event::button::KeyboardKeyId::Left;
            case SDLK_RIGHT:
                return core::event::button::KeyboardKeyId::Right;
            case SDLK_UP:
                return core::event::button::KeyboardKeyId::Up;
            case SDLK_DOWN:
                return core::event::button::KeyboardKeyId::Down;
            case SDLK_ESCAPE:
                return core::event::button::KeyboardKeyId::Escape;
            case SDLK_a:
                return core::event::button::KeyboardKeyId::A;
            case SDLK_b:
                return core::event::button::KeyboardKeyId::B;
            case SDLK_c:
                return core::event::button::KeyboardKeyId::C;
            case SDLK_d:
                return core::event::button::KeyboardKeyId::D;
            case SDLK_e:
                return core::event::button::KeyboardKeyId::E;
            case SDLK_f:
                return core::event::button::KeyboardKeyId::F;
            case SDLK_g:
                return core::event::button::KeyboardKeyId::G;
            case SDLK_h:
                return core::event::button::KeyboardKeyId::H;
            case SDLK_i:
                return core::event::button::KeyboardKeyId::I;
            case SDLK_j:
                return core::event::button::KeyboardKeyId::J;
            case SDLK_k:
                return core::event::button::KeyboardKeyId::K;
            case SDLK_l:
                return core::event::button::KeyboardKeyId::L;
            case SDLK_m:
                return core::event::button::KeyboardKeyId::M;
            case SDLK_n:
                return core::event::button::KeyboardKeyId::N;
            case SDLK_o:
                return core::event::button::KeyboardKeyId::O;
            case SDLK_p:
                return core::event::button::KeyboardKeyId::P;
            case SDLK_q:
                return core::event::button::KeyboardKeyId::Q;
            case SDLK_r:
                return core::event::button::KeyboardKeyId::R;
            case SDLK_s:
                return core::event::button::KeyboardKeyId::S;
            case SDLK_t:
                return core::event::button::KeyboardKeyId::T;
            case SDLK_u:
                return core::event::button::KeyboardKeyId::U;
            case SDLK_v:
                return core::event::button::KeyboardKeyId::V;
            case SDLK_w:
                return core::event::button::KeyboardKeyId::W;
            case SDLK_x:
                return core::event::button::KeyboardKeyId::X;
            case SDLK_y:
                return core::event::button::KeyboardKeyId::Y;
            case SDLK_z:
                return core::event::button::KeyboardKeyId::Z;
            case SDLK_0:
                return core::event::button::KeyboardKeyId::Num0;
            case SDLK_1:
                return core::event::button::KeyboardKeyId::Num1;
            case SDLK_2:
                return core::event::button::KeyboardKeyId::Num2;
            case SDLK_3:
                return core::event::button::KeyboardKeyId::Num3;
            case SDLK_4:
                return core::event::button::KeyboardKeyId::Num4;
            case SDLK_5:
                return core::event::button::KeyboardKeyId::Num5;
            case SDLK_6:
                return core::event::button::KeyboardKeyId::Num6;
            case SDLK_7:
                return core::event::button::KeyboardKeyId::Num7;
            case SDLK_8:
                return core::event::button::KeyboardKeyId::Num8;
            case SDLK_9:
                return core::event::button::KeyboardKeyId::Num9;
            case SDLK_KP_0:
                return core::event::button::KeyboardKeyId::Numpad0;
            case SDLK_KP_1:
                return core::event::button::KeyboardKeyId::Numpad1;
            case SDLK_KP_2:
                return core::event::button::KeyboardKeyId::Numpad2;
            case SDLK_KP_3:
                return core::event::button::KeyboardKeyId::Numpad3;
            case SDLK_KP_4:
                return core::event::button::KeyboardKeyId::Numpad4;
            case SDLK_KP_5:
                return core::event::button::KeyboardKeyId::Numpad5;
            case SDLK_KP_6:
                return core::event::button::KeyboardKeyId::Numpad6;
            case SDLK_KP_7:
                return core::event::button::KeyboardKeyId::Numpad7;
            case SDLK_KP_8:
                return core::event::button::KeyboardKeyId::Numpad8;
            case SDLK_KP_9:
                return core::event::button::KeyboardKeyId::Numpad9;
            case SDLK_SPACE:
                return core::event::button::KeyboardKeyId::Space;
            case SDLK_LEFTBRACKET:
                return core::event::button::KeyboardKeyId::LeftBracket;
            case SDLK_RIGHTBRACKET:
                return core::event::button::KeyboardKeyId::RightBracket;
            case SDLK_BACKQUOTE:
                return core::event::button::KeyboardKeyId::Tilda;
            case SDLK_BACKSPACE:
                return core::event::button::KeyboardKeyId::Backspace;
            case SDLK_BACKSLASH:
                return core::event::button::KeyboardKeyId::Backslash;
            case SDLK_SLASH:
                return core::event::button::KeyboardKeyId::Slash;
            case SDLK_QUOTE:
                return core::event::button::KeyboardKeyId::Quote;
            case SDLK_SEMICOLON:
                return core::event::button::KeyboardKeyId::Semicolon;
            case SDLK_MINUS:
                return core::event::button::KeyboardKeyId::Minus;
            case SDLK_EQUALS:
                return core::event::button::KeyboardKeyId::Equal;
            case SDLK_TAB:
                return core::event::button::KeyboardKeyId::Tab;
            case SDLK_CAPSLOCK:
                return core::event::button::KeyboardKeyId::CapsLock;
            case SDLK_LSHIFT:
                return core::event::button::KeyboardKeyId::LeftShift;
            case SDLK_RSHIFT:
                return core::event::button::KeyboardKeyId::RightShift;
            case SDLK_LCTRL:
                return core::event::button::KeyboardKeyId::LeftControl;
            case SDLK_RCTRL:
                return core::event::button::KeyboardKeyId::RightControl;
            case SDLK_LALT:
                return core::event::button::KeyboardKeyId::LeftAlt;
            case SDLK_RALT:
                return core::event::button::KeyboardKeyId::RightAlt;
            case SDLK_MENU:
                return core::event::button::KeyboardKeyId::Menu;
            case SDLK_COMMA:
                return core::event::button::KeyboardKeyId::Comma;
            case SDLK_PERIOD:
                return core::event::button::KeyboardKeyId::Dot;
            case SDLK_F1:
                return core::event::button::KeyboardKeyId::F1;
            case SDLK_F2:
                return core::event::button::KeyboardKeyId::F2;
            case SDLK_F3:
                return core::event::button::KeyboardKeyId::F3;
            case SDLK_F4:
                return core::event::button::KeyboardKeyId::F4;
            case SDLK_F5:
                return core::event::button::KeyboardKeyId::F5;
            case SDLK_F6:
                return core::event::button::KeyboardKeyId::F6;
            case SDLK_F7:
                return core::event::button::KeyboardKeyId::F7;
            case SDLK_F8:
                return core::event::button::KeyboardKeyId::F8;
            case SDLK_F9:
                return core::event::button::KeyboardKeyId::F9;
            case SDLK_F10:
                return core::event::button::KeyboardKeyId::F10;
            case SDLK_F11:
                return core::event::button::KeyboardKeyId::F11;
            case SDLK_F12:
                return core::event::button::KeyboardKeyId::F12;
            case SDLK_INSERT:
                return core::event::button::KeyboardKeyId::Insert;
            case SDLK_DELETE:
                return core::event::button::KeyboardKeyId::Delete;
            case SDLK_HOME:
                return core::event::button::KeyboardKeyId::Home;
            case SDLK_END:
                return core::event::button::KeyboardKeyId::End;
            case SDLK_PAGEDOWN:
                return core::event::button::KeyboardKeyId::PageDown;
            case SDLK_PAGEUP:
                return core::event::button::KeyboardKeyId::PageUp;
            case SDLK_SCROLLLOCK:
                return core::event::button::KeyboardKeyId::ScrollLock;
            case SDLK_PAUSE:
                return core::event::button::KeyboardKeyId::Pause;
            case SDLK_LESS:
                return core::event::button::KeyboardKeyId::Less;
            case SDLK_GREATER:
                return core::event::button::KeyboardKeyId::Greater;
            case SDLK_NUMLOCKCLEAR:
                return core::event::button::KeyboardKeyId::NumpadLock;
            case SDLK_KP_DIVIDE:
                return core::event::button::KeyboardKeyId::NumpadSlash;
            case SDLK_KP_MULTIPLY:
                return core::event::button::KeyboardKeyId::NumpadStar;
            case SDLK_KP_MINUS:
                return core::event::button::KeyboardKeyId::NumpadMinus;
            case SDLK_KP_PLUS:
                return core::event::button::KeyboardKeyId::NumpadPlus;
            case SDLK_KP_ENTER:
                return core::event::button::KeyboardKeyId::NumpadEnter;
            case SDLK_KP_PERIOD:
                return core::event::button::KeyboardKeyId::NumpadDot;
            default:
                GXLOGD("Unhandled mouse button, left button returned instead. " << static_cast<int>(e->key.keysym.sym))
                return core::event::button::KeyboardKeyId::Unknown;
            }
        }();
        event.source = core::event::Id::ButtonKeyboard;
        event.data = d;
        break;
    }
    case SDL_FINGERDOWN: {
        /*const core::Real x = (e->tfinger.x - 0.5f) * 2.0f * o->screen_ratio;
        const core::Real y = (0.5f - e->tfinger.y) * 2.0f;
        event = new core::event::button::Mouse(
            core::event::button::Button::KeyType::LEFT,
            core::event::button::Button::ActionType::PRESS,
            x, y);
        o->pre_x = x;
        o->pre_y = y;*/
        break;
    }
    case SDL_FINGERUP: {
        /*const core::Real x = (e->tfinger.x - 0.5f) * 2.0f * o->screen_ratio;
        const core::Real y = (0.5f - e->tfinger.y) * 2.0f;
        event = new core::event::button::Mouse(
            core::event::button::Button::KeyType::LEFT,
            core::event::button::Button::ActionType::RELEASE,
            x, y);
        o->pre_x = x;
        o->pre_y = y;*/
        break;
    }
    case SDL_FINGERMOTION: {
        /* const core::Real x = (e->tfinger.x - 0.5f) * 2.0f * o->screen_ratio;
        const core::Real y = (0.5f - e->tfinger.y) * 2.0f;
        event = new core::event::movement::Mouse(x, y, o->pre_x, o->pre_y);
        o->pre_x = x;
        o->pre_y = y;*/
        break;
    }

    case SDL_MOUSEWHEEL:
        event.source = core::event::Id::ScrollMouse;
        event.data = core::event::button::MouseScroll {
            math::Vec2(
                static_cast<core::Real>(e->wheel.x),
                static_cast<core::Real>(e->wheel.y))
        };
        break;
    case SDL_MOUSEMOTION:
        event_engine->set_mouse_movement(math::Vec2(
            convert_x_to_ratio(e->button.x),
            convert_y_to_ratio(e->button.y)));
        break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN: {
        core::event::button::MouseData d;
        event_engine->mouse_button(
            [&]() noexcept {
                switch (e->button.button) {
                case SDL_BUTTON_LEFT:
                    return core::event::button::MouseKeyId::Left;
                case SDL_BUTTON_RIGHT:
                    return core::event::button::MouseKeyId::Right;
                case SDL_BUTTON_MIDDLE:
                    return core::event::button::MouseKeyId::Middle;
                default:
                    GXLOGE("Unhandled mouse button, left button returned instead.")
                    return core::event::button::MouseKeyId::Left;
                }
            }(),
            [&]() noexcept {
                switch (e->type) {
                case SDL_MOUSEBUTTONUP:
                    return core::event::button::MouseActionId::Release;
                case SDL_MOUSEBUTTONDOWN:
                    return core::event::button::MouseActionId::Press;
                default:
                    return core::event::button::MouseActionId::Unknown;
                }
            }());
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
        case SDL_WINDOWEVENT_RESIZED: {
            core::event::system::WindowSizeChangeData d;
            d.pre_width = static_cast<core::Real>(window_width);
            d.pre_height = static_cast<core::Real>(window_height);
            d.cur_width = static_cast<core::Real>(e->window.data1);
            d.cur_height = static_cast<core::Real>(e->window.data2);
            event.data = d;
            event.source = core::event::Id::SystemWindowSizeChange;
            window_width = static_cast<unsigned int>(e->window.data1);
            window_height = static_cast<unsigned int>(e->window.data2);
            window_ratio = static_cast<core::Real>(window_width) / static_cast<core::Real>(window_height);
            half_height_inversed = 2.0f / static_cast<core::Real>(window_height);
            break;
        }
        default:
            GXTODO
            break;
        }
        break;
    default:
        GXLOGE("Unhandled event " << e->type)
        break;
    }
    if (event.source != core::event::Id::None) {
        event_engine->broadcast(event);
    }
    return 1;
}

gearoenix::system::Application::Application() noexcept
{
    GXLOGI("Constructing Gearoenix system application monomorphic interface over SDL2.")
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0) {
        GXLOGF("Failed to initialize SDL: " << SDL_GetError())
    }
#ifdef GX_USE_VULKAN
    if (vulkan::Engine::is_supported()) {
        supported_engine = render::engine::Type::VULKAN;
    }
#endif

#ifdef GX_FULLSCREEN
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    win_width = static_cast<unsigned int>(display_mode.w);
    win_height = static_cast<unsigned int>(display_mode.h);
#else
    window_width = GX_DEFAULT_WINDOW_WIDTH;
    window_height = GX_DEFAULT_WINDOW_HEIGHT;
#endif
    create_window();
    SDL_AddEventWatch(event_receiver, this);
#ifdef GX_USE_OPENGL
    if (GX_RUNTIME_USE_OPENGL_V(supported_engine)) {
        int w, h;
        SDL_GL_GetDrawableSize(window, &w, &h);
        window_width = static_cast<unsigned int>(w);
        window_height = static_cast<unsigned int>(h);
    }
#endif
    window_ratio = static_cast<core::Real>(window_width) / static_cast<core::Real>(window_height);
    half_height_inversed = 2.0f / static_cast<core::Real>(window_height);
    int mx, my;
    SDL_GetMouseState(&mx, &my);

#ifdef GX_USE_VULKAN
    if (nullptr == render_engine && supported_engine == render::engine::Type::VULKAN) {
        render_engine = new vulkan::Engine(this);
    }
#endif
#ifdef GX_USE_OPENGL_CLASS_3
    if (nullptr == render_engine && (GX_RUNTIME_USE_OPENGL_CLASS_3_V(supported_engine))) {
        render_engine = std::unique_ptr<render::engine::Engine>(glc3::engine::Engine::construct(this, supported_engine));
    }
#endif

#ifdef GX_USE_OPENGL_ES2
    if (nullptr == render_engine && supported_engine == render::engine::Type::OPENGL_ES2) {
        render_engine = std::unique_ptr<render::engine::Engine>(gles2::engine::Engine::construct(this));
    }
#endif

    if (render_engine == nullptr) {
        GXLOGF("No suitable render engine found.")
    }
    asset_manager = std::make_unique<core::asset::Manager>(this, GX_APP_DATA_NAME);
    event_engine = std::make_unique<core::event::Engine>();
    event_engine->set_mouse_position(math::Vec2(
        convert_x_to_ratio(mx), convert_y_to_ratio(my)));
}

gearoenix::system::Application::~Application() noexcept
{
    GXLOGD("Main application (SDL2) has been deleted.")
}

void gearoenix::system::Application::execute(std::unique_ptr<core::Application> app) noexcept
{
    core_application = std::move(app);
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
    core_application->update();
    render_engine->update();
#ifdef GX_USE_OPENGL
    if (GX_RUNTIME_USE_OPENGL_V(supported_engine)) {
        SDL_GL_SwapWindow(window);
    }
#endif
#ifndef GX_IN_WEB
}
core_application->terminate();
core_application = nullptr;
event_engine = nullptr;
asset_manager = nullptr;
render_engine = nullptr;
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

gearoenix::core::Real gearoenix::system::Application::convert_x_to_ratio(int x) const noexcept
{
    return (static_cast<core::Real>(x) * half_height_inversed) - window_ratio;
}

gearoenix::core::Real gearoenix::system::Application::convert_y_to_ratio(int y) const noexcept
{
    return 1.0f - (static_cast<core::Real>(y) * half_height_inversed);
}

const char* gearoenix::system::Application::get_clipboard() const noexcept
{
    return SDL_GetClipboardText();
}

#endif
