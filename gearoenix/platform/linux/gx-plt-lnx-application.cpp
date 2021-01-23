#include "gx-plt-lnx-application.hpp"
#include "../gx-plt-log.hpp"

#ifdef GX_PLATFORM_LINUX

bool gearoenix::platform::Application::fetch_events(XEvent& event) noexcept
{
    while (XPending(display) > 0) {
        XNextEvent(display, &event);
        if (event.type == ClientMessage && event.xclient.data.l[0] == close_message)
            return false;
    }
    return true;
}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
    , display(XOpenDisplay(nullptr))
    , close_message(XInternAtom(display, "WM_DELETE_WINDOW", False))
{
    window = XCreateSimpleWindow(
        display, XDefaultRootWindow(display),
        100, 100, 200, 200, 4, 0, 0);
    XMapWindow(display, window);
    XSelectInput(display, window, KeyPressMask | ButtonPressMask | ExposureMask);
    XSetWMProtocols(display, window, &close_message, 1);
}

gearoenix::platform::Application::~Application() noexcept
{
    XDestroyWindow(display, window);
    window = 0;
    XCloseDisplay(display);
    display = nullptr;
}

void gearoenix::platform::Application::run() noexcept
{
    XEvent event;
    while (fetch_events(event)) {
        GX_LOG_I("Event code" << event.type)
    }
}

#endif