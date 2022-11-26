#ifndef GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
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

    explicit Manager(engine::Engine& e) noexcept;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    virtual ~Manager() noexcept = default;

    [[nodiscard]] virtual std::shared_ptr<Builder> build(const std::string& name, core::sync::EndCaller&& end_caller) noexcept = 0;
    virtual void update() noexcept;
    virtual void window_resized() noexcept;
};
}
#endif
