#include "gx-plt-x11-application.hpp"
#ifdef GX_PLATFORM_INTERFACE_X11
#include "../../core/gx-cr-application.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-plt-x11-key.hpp"

#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../vulkan/gx-vk-loader.hpp"
#endif

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
            const auto& xce = event.xconfigure;
            if (0 < xce.width && 0 < xce.height)
                base.update_window_size(xce.width, xce.height);
            break;
        }
        case EnterNotify:
        case LeaveNotify:
        case MotionNotify: {
            const auto& me = event.xmotion;
            base.update_mouse_position(me.x, me.y);
            break;
        }
        case ButtonPress:
            base.mouse_key(convert_mouse_to_key(event.xbutton.button), key::Action::Press);
            break;
        case ButtonRelease:
            base.mouse_key(convert_mouse_to_key(event.xbutton.button), key::Action::Release);
            break;
        case KeyPress: {
            auto& ke = event.xkey;
            auto ks = XLookupKeysym(&ke, 0);
            if (NoSymbol == ks)
                break;
            wchar_t c = 0;
            Status status = 0;
            break;
        }
        case KeyRelease:
            break;
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
    XSelectInput(display, window,
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | VisibilityChangeMask | ExposureMask | StructureNotifyMask);
    XSetWMProtocols(display, window, &close_message, 1);
    XMapWindow(display, window);
    XMoveWindow(display, window, base.window_size.x, base.window_size.y);

    while (true) {
        XEvent e;
        XNextEvent(display, &e);
        if (e.type == Expose) {
            base.window_is_up = true;
            break;
        }
    }
    Window ignored_window;
    int ignored_int, pointer_x, pointer_y;
    if (XQueryPointer(
            display, root_window, &ignored_window, &ignored_window,
            &ignored_int, &ignored_int, &pointer_x, &pointer_y, reinterpret_cast<unsigned int*>(&ignored_int))) {
        base.initialize_mouse_position(pointer_x, pointer_y);
    }

    base.initialize_engine(*this);
}

gearoenix::platform::Application::~Application() noexcept
{
    base.render_engine = nullptr;
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void gearoenix::platform::Application::run(core::Application* const core_app) noexcept
{
    base.initialize_core_application(*this, core_app);
    for (fetch_events(); base.running; fetch_events()) {
        base.update();
    }
}

#ifdef GX_RENDER_VULKAN_ENABLED
std::vector<const char*> gearoenix::platform::Application::get_vulkan_extensions() const noexcept
{
    return { VK_KHR_XLIB_SURFACE_EXTENSION_NAME };
}
#endif

#endif