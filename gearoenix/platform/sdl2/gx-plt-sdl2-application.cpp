#include "gx-plt-sdl2-application.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-plt-log.hpp"
#include <SDL_vulkan.h>

static bool sdl_initialized = false;

void gearoenix::platform::Application::initialize_sdl() noexcept
{
    if (sdl_initialized)
        return;
    sdl_initialized = true;
    if (SDL_Init(
            SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_VIDEO)
        != 0) {
        GX_LOG_E("SDL_Init error: " << SDL_GetError())
        std::exit(-1);
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
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetSwapInterval(0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        if (create_gl_window(4, 6, flags))
            return;
        if (create_gl_window(4, 5, flags))
            return;
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
        return true;
    }
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
            return true;
        }
        SDL_DestroyWindow(window);
    }
    return false;
}

void gearoenix::platform::Application::fetch_events() noexcept
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        //        if (event.type == SDL_QUIT)
        //            done = true;
        //        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
        //            done = true;
    }
}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
{
    initialize_sdl();
    initialize_screen();
    initialize_window();
    initialize_mouse();
}

gearoenix::platform::Application::~Application() noexcept
{
    if (nullptr != gl_context)
        SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void gearoenix::platform::Application::run(core::Application* core_app) noexcept
{
    while (base.running) {
        fetch_events();
    }
}

std::vector<const char*> gearoenix::platform::Application::get_vulkan_extensions() noexcept
{
    std::uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, nullptr);
    std::vector<const char*> extensions(static_cast<std::size_t>(extensions_count));
    SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, extensions.data());
    return extensions;
}

#endif
