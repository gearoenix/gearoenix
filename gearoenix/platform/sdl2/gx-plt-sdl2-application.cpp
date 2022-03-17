#include "gx-plt-sdl2-application.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-plt-log.hpp"
#include <SDL_vulkan.h>

#include "../../opengl/gx-gl-loader.hpp"

static bool sdl_initialized = false;

void gearoenix::platform::Application::initialize_sdl() noexcept
{
    if (sdl_initialized)
        return;
    sdl_initialized = true;
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        GX_LOG_E("SDL_Init error: " << SDL_GetError())
        std::terminate();
    }
}

void gearoenix::platform::Application::initialize_screen() noexcept
{
    if (base.configuration.get_fullscreen()) {
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);
        base.initialize_window_size(static_cast<int>(display_mode.w), static_cast<int>(display_mode.h));
    }
    if (base.configuration.get_lanscape()) {
        SDL_SetHint(SDL_HINT_ORIENTATIONS, "Landscape");
    }
}

void gearoenix::platform::Application::initialize_window() noexcept
{
    std::uint32_t core_flags = SDL_WINDOW_SHOWN;
    core_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
    if (base.configuration.get_fullscreen()) {
        core_flags |= SDL_WINDOW_FULLSCREEN;
        core_flags |= SDL_WINDOW_BORDERLESS;
        core_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else {
        core_flags |= SDL_WINDOW_RESIZABLE;
    }
    const auto available_engines = render::engine::Engine::get_available_engines();
    if (available_engines.contains(render::engine::Type::Vulkan)) {
        std::uint32_t flags = core_flags | SDL_WINDOW_VULKAN;
        if (create_window(flags)) {
            GX_LOG_D("Gearoenix SDL2 window has been created by Vulkan setting.")
            return;
        }
    }
    if (available_engines.contains(render::engine::Type::OpenGL)) {
        std::uint32_t flags = core_flags | SDL_WINDOW_OPENGL;
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 2);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        // SDL_GL_SetSwapInterval(0);
        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        // if (create_gl_window(4, 6, flags))
        //     return;
        // if (create_gl_window(4, 5, flags))
        //     return;
        // if (create_gl_window(4, 4, flags))
        //     return;
        // if (create_gl_window(4, 3, flags))
        //     return;
        // if (create_gl_window(4, 2, flags))
        //     return;
        // if (create_gl_window(4, 1, flags))
        //     return;
        // if (create_gl_window(4, 0, flags))
        //     return;
        // if (create_gl_window(3, 3, flags))
        //     return;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        if (create_gl_window(3, 2, flags))
            return;
        if (create_gl_window(3, 1, flags))
            return;
        if (create_gl_window(3, 0, flags))
            return;
        if (create_gl_window(2, 0, flags))
            return;
    }
    GX_LOG_F("Can not create window with minimum requirements")
}

void gearoenix::platform::Application::initialize_mouse() noexcept
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    base.initialize_mouse_position(x, y);
}

bool gearoenix::platform::Application::create_window(const std::uint32_t flags) noexcept
{
    window = SDL_CreateWindow(
        base.configuration.get_application_name().c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        base.configuration.get_window_width(), base.configuration.get_window_height(),
        flags);
    return window != nullptr;
}

bool gearoenix::platform::Application::create_gl_window(const int mj, const int mn, const std::uint32_t flags) noexcept
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, mj);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mn);
    if (create_gl_sample_window(16, flags) || create_gl_sample_window(8, flags) || create_gl_sample_window(4, flags) || create_gl_sample_window(2, flags) || create_gl_sample_window(0, flags)) {
        gl_major = mj;
        gl_minor = mn;
        GX_LOG_D("OpenGL context with major: " << mj << " and minor: " << mn << " has been created.")
        return true;
    }
    GX_LOG_D("OpenGL window creatin with major: " << mj << " and minor: " << mn << " has been failed.")
    return false;
}

bool gearoenix::platform::Application::create_gl_sample_window(const int samples, const std::uint32_t flags) noexcept
{
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, samples > 0 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
    if (create_gl_depth_window(32, flags) || create_gl_depth_window(24, flags) || create_gl_depth_window(16, flags)) {
        gl_samples = samples;
        return true;
    }
    return false;
}

bool gearoenix::platform::Application::create_gl_depth_window(const int depth, const std::uint32_t flags) noexcept
{
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
    if (create_window(flags)) {
        gl_context = SDL_GL_CreateContext(window);
        if (nullptr != gl_context) {
            gl_depth = depth;
            SDL_GL_MakeCurrent(window, gl_context);
            if (gl::load_library())
                return true;
            gl::unload_library();
        }
        SDL_DestroyWindow(window);
    }
    return false;
}

void gearoenix::platform::Application::fetch_events() noexcept
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_TEXTINPUT:
            base.character_input(e.text.text[0]);
            break;
        case SDL_QUIT:
            base.going_to_be_closed();
            break;
        case SDL_APP_WILLENTERBACKGROUND:
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            base.keyboard_key(
                [&]() noexcept {
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            return key::Id::Left;
                        case SDLK_RIGHT:
                            return key::Id::Right;
                        case SDLK_UP:
                            return key::Id::Up;
                        case SDLK_DOWN:
                            return key::Id::Down;
                        case SDLK_ESCAPE:
                            return key::Id::Escape;
                        case SDLK_a:
                            return key::Id::A;
                        case SDLK_b:
                            return key::Id::B;
                        case SDLK_c:
                            return key::Id::C;
                        case SDLK_d:
                            return key::Id::D;
                        case SDLK_e:
                            return key::Id::E;
                        case SDLK_f:
                            return key::Id::F;
                        case SDLK_g:
                            return key::Id::G;
                        case SDLK_h:
                            return key::Id::H;
                        case SDLK_i:
                            return key::Id::I;
                        case SDLK_j:
                            return key::Id::J;
                        case SDLK_k:
                            return key::Id::K;
                        case SDLK_l:
                            return key::Id::L;
                        case SDLK_m:
                            return key::Id::M;
                        case SDLK_n:
                            return key::Id::N;
                        case SDLK_o:
                            return key::Id::O;
                        case SDLK_p:
                            return key::Id::P;
                        case SDLK_q:
                            return key::Id::Q;
                        case SDLK_r:
                            return key::Id::R;
                        case SDLK_s:
                            return key::Id::S;
                        case SDLK_t:
                            return key::Id::T;
                        case SDLK_u:
                            return key::Id::U;
                        case SDLK_v:
                            return key::Id::V;
                        case SDLK_w:
                            return key::Id::W;
                        case SDLK_x:
                            return key::Id::X;
                        case SDLK_y:
                            return key::Id::Y;
                        case SDLK_z:
                            return key::Id::Z;
                        case SDLK_0:
                            return key::Id::Num0;
                        case SDLK_1:
                            return key::Id::Num1;
                        case SDLK_2:
                            return key::Id::Num2;
                        case SDLK_3:
                            return key::Id::Num3;
                        case SDLK_4:
                            return key::Id::Num4;
                        case SDLK_5:
                            return key::Id::Num5;
                        case SDLK_6:
                            return key::Id::Num6;
                        case SDLK_7:
                            return key::Id::Num7;
                        case SDLK_8:
                            return key::Id::Num8;
                        case SDLK_9:
                            return key::Id::Num9;
                        case SDLK_KP_0:
                            return key::Id::Numpad0;
                        case SDLK_KP_1:
                            return key::Id::Numpad1;
                        case SDLK_KP_2:
                            return key::Id::Numpad2;
                        case SDLK_KP_3:
                            return key::Id::Numpad3;
                        case SDLK_KP_4:
                            return key::Id::Numpad4;
                        case SDLK_KP_5:
                            return key::Id::Numpad5;
                        case SDLK_KP_6:
                            return key::Id::Numpad6;
                        case SDLK_KP_7:
                            return key::Id::Numpad7;
                        case SDLK_KP_8:
                            return key::Id::Numpad8;
                        case SDLK_KP_9:
                            return key::Id::Numpad9;
                        case SDLK_SPACE:
                            return key::Id::Space;
                        case SDLK_LEFTBRACKET:
                            return key::Id::LeftBracket;
                        case SDLK_RIGHTBRACKET:
                            return key::Id::RightBracket;
                        case SDLK_BACKQUOTE:
                            return key::Id::Tilda;
                        case SDLK_BACKSPACE:
                            return key::Id::Backspace;
                        case SDLK_BACKSLASH:
                            return key::Id::Backslash;
                        case SDLK_SLASH:
                            return key::Id::Slash;
                        case SDLK_QUOTE:
                            return key::Id::Quote;
                        case SDLK_SEMICOLON:
                            return key::Id::Semicolon;
                        case SDLK_MINUS:
                            return key::Id::Minus;
                        case SDLK_EQUALS:
                            return key::Id::Equal;
                        case SDLK_TAB:
                            return key::Id::Tab;
                        case SDLK_CAPSLOCK:
                            return key::Id::CapsLock;
                        case SDLK_LSHIFT:
                            return key::Id::LeftShift;
                        case SDLK_RSHIFT:
                            return key::Id::RightShift;
                        case SDLK_LCTRL:
                            return key::Id::LeftControl;
                        case SDLK_RCTRL:
                            return key::Id::RightControl;
                        case SDLK_LALT:
                            return key::Id::LeftAlt;
                        case SDLK_RALT:
                            return key::Id::RightAlt;
                        case SDLK_MENU:
                            return key::Id::Menu;
                        case SDLK_COMMA:
                            return key::Id::Comma;
                        case SDLK_PERIOD:
                            return key::Id::Dot;
                        case SDLK_F1:
                            return key::Id::F1;
                        case SDLK_F2:
                            return key::Id::F2;
                        case SDLK_F3:
                            return key::Id::F3;
                        case SDLK_F4:
                            return key::Id::F4;
                        case SDLK_F5:
                            return key::Id::F5;
                        case SDLK_F6:
                            return key::Id::F6;
                        case SDLK_F7:
                            return key::Id::F7;
                        case SDLK_F8:
                            return key::Id::F8;
                        case SDLK_F9:
                            return key::Id::F9;
                        case SDLK_F10:
                            return key::Id::F10;
                        case SDLK_F11:
                            return key::Id::F11;
                        case SDLK_F12:
                            return key::Id::F12;
                        case SDLK_INSERT:
                            return key::Id::Insert;
                        case SDLK_DELETE:
                            return key::Id::Delete;
                        case SDLK_HOME:
                            return key::Id::Home;
                        case SDLK_END:
                            return key::Id::End;
                        case SDLK_PAGEDOWN:
                            return key::Id::PageDown;
                        case SDLK_PAGEUP:
                            return key::Id::PageUp;
                        case SDLK_SCROLLLOCK:
                            return key::Id::ScrollLock;
                        case SDLK_PAUSE:
                            return key::Id::Pause;
                        case SDLK_LESS:
                            return key::Id::Less;
                        case SDLK_GREATER:
                            return key::Id::Greater;
                        case SDLK_NUMLOCKCLEAR:
                            return key::Id::NumpadLock;
                        case SDLK_KP_DIVIDE:
                            return key::Id::NumpadSlash;
                        case SDLK_KP_MULTIPLY:
                            return key::Id::NumpadStar;
                        case SDLK_KP_MINUS:
                            return key::Id::NumpadMinus;
                        case SDLK_KP_PLUS:
                            return key::Id::NumpadPlus;
                        case SDLK_KP_ENTER:
                            return key::Id::NumpadEnter;
                        case SDLK_KP_PERIOD:
                            return key::Id::NumpadDot;
                        default:
                            GX_LOG_D("Unhandled button: " << static_cast<int>(e.key.keysym.sym))
                            return key::Id::Unknown;
                    } }(), e.type == SDL_KEYUP ? key::Action::Release : key::Action::Press);
            break;
        case SDL_FINGERDOWN: {
            break;
        }
        case SDL_FINGERUP: {
            break;
        }
        case SDL_FINGERMOTION: {
            break;
        }
        case SDL_MOUSEWHEEL:
            base.mouse_wheel(e.wheel.y);
            break;
        case SDL_MOUSEMOTION:
            base.update_mouse_position(e.motion.x, e.motion.y);
            break;
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            base.mouse_key(
                [&]() noexcept {
                    switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        return key::Id::Left;
                    case SDL_BUTTON_RIGHT:
                        return key::Id::Right;
                    case SDL_BUTTON_MIDDLE:
                        return key::Id::Middle;
                    default:
                        GX_LOG_E("Unhandled mouse button, left button returned instead." << e.button.button)
                        return key::Id::Left;
                    }
                }(),
                SDL_MOUSEBUTTONUP == e.type ? key::Action::Release : key::Action::Press);
            break;
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                base.update_window_size(
                    static_cast<int>(e.window.data1),
                    static_cast<int>(e.window.data2));
                break;
            default:
                GX_LOG_E("Unhandled windows event: " << static_cast<int>(e.window.event))
                break;
            }
            break;
        default:
            GX_LOG_E("Unhandled event " << e.type)
            break;
        }
    }
}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
{
    initialize_sdl();
    initialize_screen();
    initialize_window();
    initialize_mouse();
    base.initialize_engine(*this);
}

gearoenix::platform::Application::~Application() noexcept
{
    base.terminate();
    if (nullptr != gl_context) {
        SDL_GL_DeleteContext(gl_context);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void gearoenix::platform::Application::run(core::Application* core_app) noexcept
{
    base.initialize_core_application(*this, core_app);
    while (base.running) {
        fetch_events();
        base.update();
    }
}

void gearoenix::platform::Application::set_caption(const std::string& s) noexcept
{
    SDL_SetWindowTitle(window, s.c_str());
}

void gearoenix::platform::Application::set_window_fullscreen(const bool b) noexcept
{
    SDL_SetWindowFullscreen(window, b ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    base.configuration.set_fullscreen(b);
}

std::vector<const char*> gearoenix::platform::Application::get_vulkan_extensions() noexcept
{
    std::uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, nullptr);
    std::vector<const char*> extensions(static_cast<std::size_t>(extensions_count));
    SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, extensions.data());
    return extensions;
}

void gearoenix::platform::Application::create_vulkan_surface(
    void* vulkan_instance, void* const vulkan_data_ptr) const noexcept
{
    if (SDL_Vulkan_CreateSurface(
            window,
            reinterpret_cast<VkInstance>(vulkan_instance),
            reinterpret_cast<VkSurfaceKHR*>(vulkan_data_ptr))
        == 0)
        GX_LOG_F("Failed to create Vulkan surface.")
}

#endif
