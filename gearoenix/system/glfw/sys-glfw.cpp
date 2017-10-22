#include "sys-glfw.hpp"
#ifdef USE_GLFW
#include "../../gles2/gles2-engine.hpp"
#include "../sys-log.hpp"

std::map<GLFWwindow*, gearoenix::system::Application*> gearoenix::system::Application::event_mapper;

void gearoenix::frag::ui::MainWindow::onErrorEvent(int error_number, const char* error_description)
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

void gearoenix::system::Application::on_change_size_event(GLFWwindow*, int, int)
{
    TODO;
    //    (void) window;
    //    windowWidth = float(width);
    //    windowHeight = float(height);
    //    glViewport(0, 0, width, height);
    //    glScissor(0, 0, width, height);
    //    Container::windowSizeChanged(width, height);
    //    application->windowSizeChaned(windowWidth, windowHeight);
}

gearoenix::system::Application::Application()
{
    glfwSetErrorCallback(onErrorEvent);
    if (!glfwInit()) {
        UNEXPECTED;
    }
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

    //#ifdef FRAG_USE_OPENGL_ES_2
    //    GLuint shadowMapRenderBuffer;
    //    glGenRenderbuffers(1, &shadowMapRenderBuffer);
    //    FRAG_CHECK_FOR_GRAPHIC_API_ERROR
    //            glBindRenderbuffer(GL_RENDERBUFFER, shadowMapRenderBuffer);
    //    FRAG_CHECK_FOR_GRAPHIC_API_ERROR
    //            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 1024, 1024);
    //    FRAG_CHECK_FOR_GRAPHIC_API_ERROR
    //        #endif
    //            glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);
    //    glGenTextures(1, &shadowMapTexture);
    //    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    //#ifdef FRAG_USE_OPENGL_ES_2
    //    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadowMapRenderBuffer);
    //    FRAG_CHECK_FOR_GRAPHIC_API_ERROR
    //            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //    FRAG_CHECK_FOR_GRAPHIC_API_ERROR
    //        #else
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    //#endif
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //#ifndef FRAG_USE_OPENGL_ES_2
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    //#endif
    //#ifdef FRAG_USE_OPENGL_ES_2
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowMapTexture, 0);
    //    FRAG_CHECK_FOR_GRAPHIC_API_ERROR
    //        #else
    //    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapTexture, 0);
    //    glDrawBuffer(GL_NONE);
    //#endif
    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    {
    //        FRAG_LOG_ERROR("Error: FrameBufferObject is not complete!");
    //        FRAG_TERMINATE;
    //    }
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_SCISSOR_TEST);
    //    glEnable(GL_STENCIL_TEST);
    //#if !defined(FRAG_USE_OPENGL_ES_2) && !defined(FRAG_USE_OPENGL_ES_3)
    //    glEnable(GL_ALPHA_TEST);
    //#endif
    //    glViewport(0, 0, 1024, 1024);
    //    glScissor(0, 0, 1024, 1024);
    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //    glViewport(0, 0, width, height);
    //    glScissor(0, 0, width, height);
    //    core::ContextManager::ioThreadInitialized.lock();
    //    core::ContextManager::auThreadInitialized.lock();
    //    Container::initialize();
    //    application->initialize();
    //#ifndef ANDROID
    //    glfwSetFramebufferSizeCallback(window, onChangeSizeEvent);
    //#ifndef FRAG_TEST_MODE
    ////	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //#endif
    //    while(!glfwWindowShouldClose(window))
    //    {
    //        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //        core::ContextManager::glSynchronizer.lock();
    //        for(auto &f: core::ContextManager::glActions)
    //        {
    //            f();
    //        }
    //        core::ContextManager::glActions.clear();
    //        core::ContextManager::glSynchronizer.release();
    //        application->update();
    //        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //        Container::render();
    //        glFinish();
    //        glfwSwapBuffers(window);
    //        glfwPollEvents();
    //    }
    //    application->terminate();
    //    core::ContextManager::terminate();
    //    glfwDestroyWindow(window);
    //    glfwTerminate();
    //#endif
}

gearoenix::system::Application::~Application()
{
    TODO;
}

void gearoenix::system::Application::execute(core::Application* app)
{
    core_app = app;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        render_engine->update();
        SDL_GL_SwapWindow(window);
    }
    SDL_DelEventWatch(event_receiver, this);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
    TODO;
    return 1.7f;
}

unsigned int gearoenix::system::Application::get_width() const
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    (void)h;
    return static_cast<unsigned int>(w);
}

unsigned int gearoenix::system::Application::get_height() const
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    (void)w;
    return static_cast<unsigned int>(h);
}

#endif
