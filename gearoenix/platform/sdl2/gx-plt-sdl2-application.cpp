#include "gx-plt-sdl2-application.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-plt-log.hpp"

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        GX_LOG_E("SDL_Init error: " << SDL_GetError())
        std::exit(-1);
    }
    const auto available_engines = render::engine::Engine::get_available_engines();
    int window_flags = SDL_WINDOW_RESIZABLE;
    window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
    if (available_engines.contains(render::engine::Type::Vulkan))
        window_flags |= SDL_WINDOW_VULKAN;
}

gearoenix::platform::Application::~Application() noexcept
{
}

void gearoenix::platform::Application::run(core::Application* core_app) noexcept
{
}

#endif
