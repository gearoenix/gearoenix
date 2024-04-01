#ifndef GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct Builder;
struct Manager {
protected:
    engine::Engine& e;

    explicit Manager(engine::Engine& e);
    [[nodiscard]] virtual std::shared_ptr<Builder> build_v(const std::string& name, core::job::EndCaller<>&& end_caller) = 0;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    virtual ~Manager() = default;

    [[nodiscard]] std::shared_ptr<Builder> build(const std::string& name, core::job::EndCaller<>&& end_caller);

    virtual void update();
    virtual void window_resized();
};
}
#endif
