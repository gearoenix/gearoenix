#include "sys-linux-app.hpp"
#ifdef IN_LINUX
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../render/rnd-engine.hpp"
#include "../sys-file.hpp"
#include "../sys-log.hpp"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

gearoenix::system::Application::Application()
{
    const xcb_setup_t* setup;
    xcb_screen_iterator_t iter;
    int scr;
    connection = xcb_connect(NULL, &scr);
    if (connection == NULL) {
        LOGF("Could not find a compatible Vulkan ICD!");
    }
    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);
    screen = iter.data;
    uint32_t value_mask, value_list[32];
    window = xcb_generate_id(connection);
    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_RESIZE_REDIRECT;
    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0,
        0, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
        value_mask, value_list);
    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie, 0);
    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    atom_wm_delete_window = xcb_intern_atom_reply(connection, cookie2, 0);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom,
        4, 32, 1, &(*atom_wm_delete_window).atom);
    std::string window_title(APPLICATION_NAME);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, window_title.size(),
        window_title.c_str());
    free(reply);
    xcb_map_window(connection, window);
    assetmgr = new core::asset::Manager(this, "data.gx3d");
    render_engine = new render::Engine(this);
    assetmgr->initialize();
    /// todo intialize in here
}

gearoenix::system::Application::~Application()
{
    delete core_app;
    delete render_engine;
    delete assetmgr;
}

void gearoenix::system::Application::execute(core::Application* ca)
{
    core_app = ca;
    xcb_flush(connection);
    while (!quit) {
        // auto tStart = std::chrono::high_resolution_clock::now();
        // if (viewUpdated) {
        // 	viewUpdated = false;
        // 	viewChanged();
        // }
        xcb_generic_event_t* event;
        while ((event = xcb_poll_for_event(connection))) {
            if ((XCB_DESTROY_NOTIFY == (event->response_type & 0x7f)) || ((XCB_CLIENT_MESSAGE == (event->response_type & 0x7f)) && ((*(xcb_client_message_event_t*)event).data.data32[0] == (*atom_wm_delete_window).atom))) {
                quit = true;
            }
            handle(event);
            free(event);
        }
        core_app->update();
        render_engine->update();
        // render();
        // frameCounter++;
        // auto tEnd = std::chrono::high_resolution_clock::now();
        // auto tDiff = std::chrono::duration<double, std::milli>(tEnd -
        // tStart).count();
        // frameTimer = tDiff / 1000.0f;
        // camera.update(frameTimer);
        // if (camera.moving())
        // {
        // 	viewUpdated = true;
        // }
        // // Convert to clamped timer value
        // if (!paused)
        // {
        // 	timer += timerSpeed * frameTimer;
        // 	if (timer > 1.0)
        // 	{
        // 		timer -= 1.0f;
        // 	}
        // }
        // fpsTimer += (float)tDiff;
        // if (fpsTimer > 1000.0f)
        // {
        // 	if (!enableTextOverlay)
        // 	{
        // 		std::string windowTitle = getWindowTitle();
        // 		xcb_change_property(connection, XCB_PROP_MODE_REPLACE,
        // 			window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
        // 			windowTitle.size(), windowTitle.c_str());
        // 	}
        // 	lastFPS = frameCounter;
        // 	updateTextOverlay();
        // 	fpsTimer = 0.0f;
        // 	frameCounter = 0;
        // }
    }
    core_app->terminate();
    render_engine->terminate();
    // vkDeviceWaitIdle(device);
}

xcb_connection_t* gearoenix::system::Application::get_connection() const
{
    return connection;
}

xcb_window_t gearoenix::system::Application::get_window() const
{
    return window;
}

gearoenix::core::asset::Manager*
gearoenix::system::Application::get_asset_manager()
{
    return assetmgr;
}

const gearoenix::core::Application* gearoenix::system::Application::get_core_app() const
{
    return core_app;
}

const gearoenix::render::Engine* gearoenix::system::Application::get_render_engine() const
{
    return render_engine;
}

gearoenix::render::Engine* gearoenix::system::Application::get_render_engine()
{
    return render_engine;
}

void gearoenix::system::Application::handle(const xcb_generic_event_t* event)
{
    switch (event->response_type & 0x7f) {
    case XCB_CONFIGURE_NOTIFY:
        render_engine->window_changed();
        break;
    case XCB_RESIZE_REQUEST: {
        const xcb_resize_request_event_t* resize = reinterpret_cast<const xcb_resize_request_event_t*>(event);
        if (resize->width > 0 && resize->height > 0) {
            winratio = static_cast<core::Real>(resize->width) / static_cast<core::Real>(resize->height);
        }
        render_engine->window_changed();
        break;
    }
    }
}

gearoenix::core::Real gearoenix::system::Application::get_window_ratio() const
{
    return winratio;
}
#endif
