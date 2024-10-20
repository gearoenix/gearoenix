#include "gx-plt-sdl2-application.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-plt-sdl2-key.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../opengl/gx-gl-loader.hpp"
#endif
#include <SDL2/SDL_vulkan.h>
#ifdef GX_PLATFORM_WEBASSEMBLY
#include <emscripten.h>
#endif

static bool sdl_initialized = false;

void gearoenix::platform::Application::initialize_sdl()
{
    if (sdl_initialized)
        return;
    sdl_initialized = true;
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        GX_LOG_F("SDL_Init error: " << SDL_GetError());
    }
}

void gearoenix::platform::Application::initialize_screen()
{
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    base.screen_size = math::Vec2(display_mode.w, display_mode.h);
    const auto& config = RuntimeConfiguration::get(this);
    if (config.get_fullscreen()) {
        base.initialize_window_size(base.screen_size.x, base.screen_size.y);
    }
    if (config.get_landscape()) {
        SDL_SetHint(SDL_HINT_ORIENTATIONS, "Landscape");
    }
}

void gearoenix::platform::Application::initialize_window()
{
    std::uint32_t core_flags = SDL_WINDOW_SHOWN;
    core_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
    if (const auto& config = RuntimeConfiguration::get(this); config.get_fullscreen()) {
        core_flags |= SDL_WINDOW_FULLSCREEN;
        core_flags |= SDL_WINDOW_BORDERLESS;
        core_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else {
        core_flags |= SDL_WINDOW_RESIZABLE;
    }
    const auto available_engines = render::engine::Engine::get_available_engines();
#ifdef GX_RENDER_VULKAN_ENABLED
    if (available_engines.contains(render::engine::Type::Vulkan)) {
        std::uint32_t flags = core_flags | SDL_WINDOW_VULKAN;
        if (create_window(flags)) {
            GX_LOG_D("Gearoenix SDL2 window has been created by Vulkan setting.");
            return;
        }
    }
#endif
#ifdef GX_RENDER_OPENGL_ENABLED
    if (available_engines.contains(render::engine::Type::OpenGL)) {
        std::uint32_t flags = core_flags | SDL_WINDOW_OPENGL;
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
#ifdef GX_DEBUG_MODE
        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
        SDL_GL_SetSwapInterval(1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        //        if (create_gl_window(3, 2, flags))
        //            return;
        if (create_gl_window(3, 1, flags))
            return;
        if (create_gl_window(3, 0, flags))
            return;
        if (create_gl_window(2, 0, flags))
            return;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        if (create_gl_window(4, 4, flags))
            return;
        if (create_gl_window(4, 3, flags))
            return;
        if (create_gl_window(4, 2, flags))
            return;
        if (create_gl_window(4, 1, flags))
            return;
        if (create_gl_window(4, 0, flags))
            return;
        if (create_gl_window(3, 3, flags))
            return;
    }
#endif
    GX_LOG_F("Can not create window with minimum requirements");
}

void gearoenix::platform::Application::initialize_mouse()
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    base.initialize_mouse_position(x, y);
}

bool gearoenix::platform::Application::create_window(const std::uint32_t flags)
{
    const auto& config = RuntimeConfiguration::get(this);
    window = SDL_CreateWindow(
        config.get_application_name().c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        config.get_window_width(), config.get_window_height(),
        flags);
    return window != nullptr;
}

#ifdef GX_RENDER_OPENGL_ENABLED
bool gearoenix::platform::Application::create_gl_window(const int mj, const int mn, const std::uint32_t flags)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, mj);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mn);
    if (create_gl_sample_window(0, flags)) {
        gl_major = mj;
        gl_minor = mn;
        GX_LOG_D("OpenGL context with major: " << mj << " and minor: " << mn << " has been created.");
        return true;
    }
    GX_LOG_D("OpenGL window creating with major: " << mj << " and minor: " << mn << " has been failed.");
    return false;
}

bool gearoenix::platform::Application::create_gl_sample_window(const int samples, const std::uint32_t flags)
{
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, samples > 0 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
    if (create_gl_depth_window(24, flags) || create_gl_depth_window(16, flags)) {
        gl_samples = samples;
        return true;
    }
    return false;
}

bool gearoenix::platform::Application::create_gl_depth_window(const int depth, const std::uint32_t flags)
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
#endif

void gearoenix::platform::Application::fetch_events()
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
            base.keyboard_key(convert_sdl_key(e.key.keysym.sym), key::Action::Press);
            break;
        case SDL_KEYUP:
            base.keyboard_key(convert_sdl_key(e.key.keysym.sym), key::Action::Release);
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
            base.mouse_key(convert_sdl_mouse_key(e.button.button), key::Action::Release);
            break;
        case SDL_MOUSEBUTTONDOWN:
            base.mouse_key(convert_sdl_mouse_key(e.button.button), key::Action::Press);
            break;
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                base.update_window_size(static_cast<int>(e.window.data1), static_cast<int>(e.window.data2));
                break;
            default:
                GX_LOG_E("Unhandled windows event: " << static_cast<int>(e.window.event));
                break;
            }
            break;
        default:
            GX_LOG_E("Unhandled event " << e.type);
            break;
        }
    }
}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF)
    : base(GX_MAIN_ENTRY_ARGS)
{
    initialize_sdl();
    initialize_screen();
    initialize_window();
    initialize_mouse();
    base.initialize_engine(*this);
}

gearoenix::platform::Application::~Application()
{
    base.terminate();
#ifdef GX_RENDER_OPENGL_ENABLED
    if (nullptr != gl_context) {
        SDL_GL_DeleteContext(gl_context);
    }
#endif
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#ifdef GX_PLATFORM_WEBASSEMBLY
static void gearoenix_platform_application_loop(void* const arg)
{
    reinterpret_cast<gearoenix::platform::Application*>(arg)->loop();
}
#endif

void gearoenix::platform::Application::run(core::Application* core_app)
{
    base.initialize_core_application(*this, core_app);

#ifdef GX_PLATFORM_WEBASSEMBLY
    emscripten_set_main_loop_arg(gearoenix_platform_application_loop, this, 0, 1);
#else
    while (base.running) {
        loop();
    }
#endif
}

void gearoenix::platform::Application::loop()
{
    fetch_events();
    base.update();
}

void gearoenix::platform::Application::set_caption(const std::string& s)
{
    SDL_SetWindowTitle(window, s.c_str());
}

void gearoenix::platform::Application::set_window_fullscreen(const bool b)
{
    SDL_SetWindowFullscreen(window, b ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    RuntimeConfiguration::get(this).set_fullscreen(b);
}

#ifdef GX_RENDER_VULKAN_ENABLED
std::vector<const char*> gearoenix::platform::Application::get_vulkan_extensions() const
{
    std::uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, nullptr);
    std::vector<const char*> extensions(static_cast<std::size_t>(extensions_count));
    SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, extensions.data());
    return extensions;
}

void gearoenix::platform::Application::create_vulkan_surface(
    void* vulkan_instance, void* const vulkan_data_ptr) const
{
    if (SDL_Vulkan_CreateSurface(
            window,
            reinterpret_cast<VkInstance>(vulkan_instance),
            reinterpret_cast<VkSurfaceKHR*>(vulkan_data_ptr))
        == SDL_FALSE)
        GX_LOG_F("Failed to create Vulkan surface.");
}
#endif

#endif
