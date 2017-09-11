#include "../../core/build-configuration.hpp"

#ifdef IN_ANDROID

#include "and-app.hpp"
#include <android_native_app_glue.h>
#include <string>
#include "../../core/application.hpp"
#include "../../render/render-engine.hpp"
#include "../log.hpp"
#include "../sys-file.hpp"

void gearoenix::nufrag::system::Application::handle_cmd(android_app *app, int32_t cmd) {
    auto sys_app = reinterpret_cast<Application *>(app->userData);
    if (sys_app != nullptr) {
        sys_app->handle(cmd);
        return;
    }
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            app->userData = new Application(app);
            break;
        default:
            LOGI(std::string("event not handled: ") + std::to_string(cmd));
    }
}

void android_main(struct android_app *app) {
    app_dummy();
    app->onAppCmd = gearoenix::nufrag::system::Application::handle_cmd;
    int events;
    android_poll_source *source;
    do {
        if (ALooper_pollAll(1, nullptr, &events, (void **) &source) >= 0) {
            if (source != nullptr)
                source->process(app, source);
        }
        if (app->userData != nullptr) {
            break;
        }
    } while (app->destroyRequested == 0);
    auto sys_app = reinterpret_cast<gearoenix::nufrag::system::Application *>(app->userData);
    sys_app->execute();
    delete sys_app;
}

gearoenix::nufrag::system::Application::Application(android_app *and_app) : and_app(and_app) {
}

gearoenix::nufrag::system::Application::~Application() {
    render_engine = nullptr;
    core_app = nullptr;
}

android_app *gearoenix::nufrag::system::Application::get_android_app() const {
    return and_app;
}

const std::shared_ptr<gearoenix::nufrag::core::Application> &
gearoenix::nufrag::system::Application::get_core_app() const {
    return core_app;
}

const std::shared_ptr<gearoenix::nufrag::render::Engine> &
gearoenix::nufrag::system::Application::get_render_engine() const {
    return render_engine;
}

void gearoenix::nufrag::system::Application::execute() {
    std::lock_guard<std::mutex> exe_lock(execution_lock);
    init();
    int events;
    android_poll_source *source;
    do {
        if (ALooper_pollAll(is_active ? 0 : 1, nullptr, &events, (void **) &source) >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
        if (is_active) {
            render_engine->update();
            core_app->update();
        }
    } while (and_app->destroyRequested == 0 && is_alive);
    (void) exe_lock;
}

const std::shared_ptr<gearoenix::nufrag::system::File> &
gearoenix::nufrag::system::Application::get_asset() const {
    return asset;
}

void gearoenix::nufrag::system::Application::handle(int32_t cmd) {
    switch (cmd) {
        case APP_CMD_LOST_FOCUS:
            if(core_app != nullptr) core_app->terminate();
            if(render_engine != nullptr) render_engine->terminate();
            asset = nullptr;
            core_app = nullptr;
            render_engine = nullptr;
            is_active = false;
            break;
        case APP_CMD_INIT_WINDOW:
            if(!is_active) {
                init();
                render_engine->window_changed();
                is_active = true;
            }
            break;
        default:
            LOGI(std::string("event not handled: ") + std::to_string(cmd));
    }
}

void gearoenix::nufrag::system::Application::init() {
    asset = std::shared_ptr<File>(new File(this));
    render_engine = std::shared_ptr<render::Engine>(new render::Engine(this));
    core_app = std::shared_ptr<core::Application>(new core::Application(this));
}

#endif
