#ifndef GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#define GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_ANDROID
#include <memory>
#include <mutex>
struct android_app;
namespace gearoenix {
namespace render {
class Engine;
}
namespace core {
class Application;
}
namespace system {
class File;
class Application {
private:
    android_app *and_app;
    std::shared_ptr<File> asset;
    std::shared_ptr<render::Engine> render_engine;
    std::shared_ptr<core::Application> core_app;
    std::mutex execution_lock;
    bool is_alive = true, is_active = true;
    void handle(int32_t cmd);
    void init();
public:
    Application(struct android_app *and_app);
    ~Application();
    void execute();
    android_app *get_android_app() const;
    const std::shared_ptr<core::Application> &get_core_app() const;
    const std::shared_ptr<render::Engine> &get_render_engine() const;
    const std::shared_ptr<File> &get_asset() const;
    static void handle_cmd(android_app *app, int32_t cmd);
};
}
}
#endif // IN_ANDROID
#endif // GEAROENIX_SYSTEM_ANDROID_HPP
