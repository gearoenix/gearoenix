#include "gx-plt-lnx-application.hpp"
#include "../../core/gx-cr-application.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-plt-log.hpp"

#ifdef GX_PLATFORM_LINUX

void gearoenix::platform::Application::fetch_events() noexcept
{
    while (XPending(display) > 0) {
        XEvent event;
        XNextEvent(display, &event);
        if (event.type == ClientMessage && event.xclient.data.l[0] == close_message) {
            base.running = false;
            return;
        }
    }
}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
    , display(XOpenDisplay(nullptr))
    , screen(XDefaultScreenOfDisplay(display))
    , window(XCreateSimpleWindow(
          display, XDefaultRootWindow(display), 0, 0,
          config.get_window_width(), config.get_window_height(), 4, 0, 0))
    , close_message(XInternAtom(display, "WM_DELETE_WINDOW", False))
{
    base.initialize_window_size(config.get_window_width(), config.get_window_height());
    base.initialize_window_position(
        (screen->width - static_cast<int>(config.get_window_width())) / 2,
        (screen->height - static_cast<int>(config.get_window_height())) / 2);
    XMapWindow(display, window);
    XMoveWindow(display, window, base.window_x, base.window_y);
    XSelectInput(display, window, KeyPressMask | ButtonPressMask | ExposureMask);
    XSetWMProtocols(display, window, &close_message, 1);
    base.render_engine = render::engine::Engine::construct(*this);
    base.initialize_imgui();
}

gearoenix::platform::Application::~Application() noexcept
{
    base.render_engine = nullptr;
    XDestroyWindow(display, window);
    window = 0;
    XCloseDisplay(display);
    display = nullptr;
}

void gearoenix::platform::Application::run(core::Application* const core_app) noexcept
{
    base.core_application = nullptr == core_app ? std::make_unique<core::Application>(this) : std::unique_ptr<core::Application>(core_app);
    for (fetch_events(); base.running; fetch_events()) {
        base.update();
    }
}

#endif