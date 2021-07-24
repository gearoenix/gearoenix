#include "gx-bgfx-engine.hpp"
#ifdef GX_RENDER_BGFX_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"

#ifdef GX_PLATFORM_INTERFACE_SDL2
#include <SDL_syswm.h>
#endif

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

static void initialize_platform(gearoenix::platform::Application& platform_application) noexcept
{
#ifdef GX_PLATFORM_INTERFACE_SDL2
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(platform_application.get_window(), &wmi))
        GX_LOG_F("Can not get window manager info from SDL2.")

    bgfx::PlatformData pd;
    GX_SET_ZERO(pd)
#ifdef GX_PLATFORM_WINDOWS
    pd.nwh = wmi.info.win.window;
#elif defined(GX_PLATFORM_LINUX)
    pd.ndt = wmi.info.win.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#else
#error "Unsupported platform"
#endif
    bgfx::setPlatformData(pd);
#else
#error "Unexpected platform."
#endif
}

gearoenix::bgfx::Engine::Engine(platform::Application& platform_application) noexcept
    : render::engine::Engine(render::engine::Type::BGFX, platform_application)
{
    initialize_platform(platform_application);

    ::bgfx::renderFrame();
    ::bgfx::init();
    ::bgfx::reset(
        static_cast<std::uint32_t>(platform_application.get_base().get_window_width()),
        static_cast<std::uint32_t>(platform_application.get_base().get_window_height()),
        BGFX_RESET_VSYNC);
#ifdef GX_DEBUG_MODE
    ::bgfx::setDebug(BGFX_DEBUG_TEXT);
#endif
    ::bgfx::setViewRect(
        0, 0, 0,
        static_cast<std::uint16_t>(platform_application.get_base().get_window_width()),
        static_cast<std::uint16_t>(platform_application.get_base().get_window_height()));
    ::bgfx::setViewClear(
        0,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x443355FF, 1.0f, 0);
    ::bgfx::touch(0);
}

gearoenix::bgfx::Engine::~Engine() noexcept
{
    ::bgfx::shutdown();
}

void gearoenix::bgfx::Engine::start_frame() noexcept
{
    render::engine::Engine::start_frame();
}

void gearoenix::bgfx::Engine::end_frame() noexcept
{
    render::engine::Engine::end_frame();
    ::bgfx::frame();
}

void gearoenix::bgfx::Engine::upload_imgui_fonts() noexcept
{
    GX_TODO
}

void gearoenix::bgfx::Engine::create_mesh(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
}

#endif