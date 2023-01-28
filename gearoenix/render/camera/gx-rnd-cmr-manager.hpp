#ifndef GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROENIX_RENDER_CAMERA_MANAGER_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../gx-rnd-resolution.hpp"
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
    std::function<void(core::ecs::entity_id_t, const Resolution&)> resolution_cfg_listener_with_entity_id_v = [](auto, const auto&) {};
    std::function<void(Builder&, const Resolution&)> resolution_cfg_listener_with_entity_builder_v = [](auto&, const auto&) {};

    explicit Manager(engine::Engine& e) noexcept;
    [[nodiscard]] virtual std::shared_ptr<Builder> build_v(const std::string& name, core::sync::EndCaller&& end_caller) noexcept = 0;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    virtual ~Manager() noexcept = default;

    [[nodiscard]] std::shared_ptr<Builder> build(const std::string& name, core::sync::EndCaller&& end_caller) noexcept;

    virtual void update() noexcept;
    virtual void window_resized() noexcept;
};
}
#endif
