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
        switch (event.type) {
        case ClientMessage: {
            if (event.xclient.data.l[0] == close_message) {
                base.running = false;
                return;
            }
            break;
        }
        case ConfigureNotify: {
            const XConfigureEvent xce = event.xconfigure;
            if (0 != xce.width && 0 != xce.height)
                base.update_window_size(xce.width, xce.height);
            break;
        }
        }
    }
}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
    , display(XOpenDisplay(nullptr))
    , screen(XDefaultScreenOfDisplay(display))
    , root_window(XRootWindowOfScreen(screen))
    , black_pixel(XBlackPixelOfScreen(screen))
    , window(XCreateSimpleWindow(
          display,
          root_window,
          0,
          0,
          config.get_window_width(),
          config.get_window_height(),
          0,
          black_pixel,
          black_pixel))
    , close_message(XInternAtom(display, "WM_DELETE_WINDOW", False))
{
    base.initialize_window_position(
        (screen->width - static_cast<int>(config.get_window_width())) / 2,
        (screen->height - static_cast<int>(config.get_window_height())) / 2);
    XSelectInput(display, window, KeyPressMask | ButtonPressMask | ExposureMask | StructureNotifyMask);
    XSetWMProtocols(display, window, &close_message, 1);
    XMapWindow(display, window);
    XMoveWindow(display, window, base.window_x, base.window_y);

    while (true) {
        XEvent e;
        XNextEvent(display, &e);
        if (e.type == Expose) {
            base.window_is_up = true;
            break;
        }
    }

    base.render_engine = render::engine::Engine::construct(*this);
}

gearoenix::platform::Application::~Application() noexcept
{
    base.render_engine = nullptr;
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void gearoenix::platform::Application::run(core::Application* const core_app) noexcept
{
    base.core_application = nullptr == core_app ? std::make_unique<core::Application>(this) : std::unique_ptr<core::Application>(core_app);
    for (fetch_events(); base.running; fetch_events()) {
        base.update();
    }
}

#endif