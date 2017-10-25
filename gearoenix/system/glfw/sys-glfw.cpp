#include "sys-glfw.hpp"
#ifdef USE_GLFW
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../gles2/gles2-engine.hpp"
#include "../sys-log.hpp"

std::map<GLFWwindow*, gearoenix::system::Application*> gearoenix::system::Application::event_mapper;

void gearoenix::system::Application::on_error_event(int error_number, const char* error_description)
{
    LOGF("Error number is: " << error_number << "Error description: " << error_description);
}

void gearoenix::system::Application::on_key_event(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    (void)scanCode;
    (void)mods;
    (void)window;
    switch (action) {
    case (GLFW_PRESS):
        switch (key) {
        case (GLFW_KEY_ESCAPE):
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case (GLFW_KEY_F1):
            TODO;
            break;
        case (GLFW_KEY_F2):
            TODO;
            break;
        case (GLFW_KEY_W):
            TODO;
            break;
        case (GLFW_KEY_A):
            TODO;
            break;
        case (GLFW_KEY_S):
            TODO;
            break;
        case (GLFW_KEY_D):
            TODO;
            break;
        case (GLFW_KEY_UP):
            TODO;
            break;
        case (GLFW_KEY_DOWN):
            TODO;
            break;
        case (GLFW_KEY_LEFT):
            TODO;
            break;
        case (GLFW_KEY_RIGHT):
            TODO;
            break;
        case (GLFW_KEY_PAGE_UP):
            TODO;
            break;
        case (GLFW_KEY_PAGE_DOWN):
            TODO;
            break;
        default:
            break;
        }
        break;
    case (GLFW_RELEASE):
        switch (key) {
        case (GLFW_KEY_ESCAPE):
            break;
        case (GLFW_KEY_W):
            TODO;
            break;
        case (GLFW_KEY_A):
            TODO;
            break;
        case (GLFW_KEY_S):
            TODO;
            break;
        case (GLFW_KEY_D):
            TODO;
            break;
        case (GLFW_KEY_UP):
            TODO;
            break;
        case (GLFW_KEY_DOWN):
            TODO;
            break;
        case (GLFW_KEY_LEFT):
            TODO;
            break;
        case (GLFW_KEY_RIGHT):
            TODO;
            break;
        default:
            break;
        }
        break;
    case (GLFW_REPEAT):
        break;
    default:
        break;
    }
}

void gearoenix::system::Application::on_mouse_key_event(GLFWwindow*, int, int, int)
{
    TODO;
    //    (void) window;
    //    (void) mods;
    //    float ex = float(((lastCursorXposition / windowWidth) - 0.5f) * (windowWidth / windowHeight));
    //    float ey = float(-((lastCursorYposition / windowHeight) - 0.5f));
    //    switch(action)
    //    {
    //    case GLFW_PRESS:
    //        switch(button)
    //        {
    //        case GLFW_MOUSE_BUTTON_LEFT:
    //            mouseState = L_DOWN;
    //            if(EventManager::event(EventManager::LEFT_MOUSE_DOWN, ex, ey)){}
    //            application->buttonPressed(core::ApplicationUnit::leftMouseButton);
    //            break;
    //        case GLFW_MOUSE_BUTTON_RIGHT:
    //            application->buttonPressed(core::ApplicationUnit::rightMouseButton);
    //            break;
    //        case GLFW_MOUSE_BUTTON_MIDDLE:
    //            application->buttonPressed(core::ApplicationUnit::middleMouseButton);
    //            break;
    //        default:
    //            break;
    //        }
    //        break;
    //    case GLFW_RELEASE:
    //        switch(button)
    //        {
    //        case GLFW_MOUSE_BUTTON_LEFT:
    //            mouseState = NONE;
    //            if(EventManager::event(EventManager::LEFT_MOUSE_UP, ex, ey)){}
    //            application->buttonReleased(core::ApplicationUnit::leftMouseButton);
    //            break;
    //        case GLFW_MOUSE_BUTTON_RIGHT:
    //            application->buttonReleased(core::ApplicationUnit::rightMouseButton);
    //            break;
    //        case GLFW_MOUSE_BUTTON_MIDDLE:
    //            application->buttonReleased(core::ApplicationUnit::middleMouseButton);
    //            break;
    //        default:
    //            break;
    //        }
    //        break;
    //    default:
    //        break;
    //    }
}

void gearoenix::system::Application::on_cursor_move_event(GLFWwindow*, double, double)
{
    TODO;
    //    (void) window;
    //    float ex = float(((x / windowWidth) - 0.5f) * (windowWidth / windowHeight));
    //    float ey = float(-((y / windowHeight) - 0.5f));
    //    switch (mouseState) {
    //    case L_DOWN:
    //        if(EventManager::event(EventManager::MOUSE_MOVE_OVER, ex, ey)) break;
    //        application->mouseMoved((float)(x - lastCursorXposition),(float)(y - lastCursorYposition));
    //        break;
    //    default:
    //        application->mouseMoved((float)(x - lastCursorXposition),(float)(y - lastCursorYposition));
    //        break;
    //    }
    //    lastCursorXposition = x;
    //    lastCursorYposition = y;
}

void gearoenix::system::Application::on_scroll(GLFWwindow*, double, double)
{
    TODO;
    //    application->zoom(float(y));
}

void gearoenix::system::Application::on_change_size_event(GLFWwindow* winid, int w, int h)
{
    event_mapper[winid]->win_width = w;
    event_mapper[winid]->win_height = h;
    TODO;
}

gearoenix::system::Application::Application()
{
    glfwSetErrorCallback(on_error_event);
    if (!glfwInit()) {
        UNEXPECTED;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
#ifdef DEBUG_MODE
    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, APPLICATION_NAME, 0, NULL);
#else
    auto pri_mon = glfwGetPrimaryMonitor();
    auto vid_mod = glfwGetVideoMode(pri_mon);
    window = glfwCreateWindow(vid_mod->width, vid_mod->height, "Gearoenix Game Engine", pri_mon, NULL);
#endif
    if (!window) {
        glfwTerminate();
        UNEXPECTED;
    }
    event_mapper[window] = this;
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        UNEXPECTED;
    }
    render_engine = new gles2::Engine(this);
    glfwSetKeyCallback(window, on_key_event);
    glfwSetMouseButtonCallback(window, on_mouse_key_event);
    glfwSetCursorPosCallback(window, on_cursor_move_event);
    glfwSetScrollCallback(window, on_scroll);
    glfwGetFramebufferSize(window, &win_width, &win_height);
    astmgr = new core::asset::Manager(this, "data.gx3d");
    astmgr->initialize();
}

gearoenix::system::Application::~Application()
{
    TODO;
}

void gearoenix::system::Application::execute(core::Application* app)
{
    core_app = app;
    glfwSetFramebufferSizeCallback(window, on_change_size_event);
    while (!glfwWindowShouldClose(window)) {
        render_engine->update();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}

const gearoenix::core::Application* gearoenix::system::Application::get_core_app() const
{
    return core_app;
}

gearoenix::core::Application* gearoenix::system::Application::get_core_app()
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

gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager()
{
    return astmgr;
}

const gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager() const
{
    return astmgr;
}

gearoenix::core::Real gearoenix::system::Application::get_window_ratio() const
{
    return static_cast<core::Real>(win_width) / static_cast<core::Real>(win_height);
}

unsigned int gearoenix::system::Application::get_width() const
{
    return static_cast<unsigned int>(win_width);
}

unsigned int gearoenix::system::Application::get_height() const
{
    return static_cast<unsigned int>(win_height);
}

#endif
