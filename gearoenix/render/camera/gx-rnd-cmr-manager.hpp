#ifndef GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#include "gx-rnd-cmr-builder.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct Builder;
struct Manager final {
private:
    engine::Engine& e;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept = default;

    [[nodiscard]] std::shared_ptr<Builder> build() noexcept;
};
}
#endif
