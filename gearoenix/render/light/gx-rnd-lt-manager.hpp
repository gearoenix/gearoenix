#ifndef GEAROENIX_RENDER_LIGHT_MANAGER_HPP
#define GEAROENIX_RENDER_LIGHT_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::light {
struct Builder;
struct Manager {
    engine::Engine& e;

public:
    Manager(engine::Engine& e) noexcept;
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    virtual ~Manager() noexcept;
    [[nodiscard]] virtual std::shared_ptr<Builder> build_directional(
        const std::string& name,
        const core::sync::EndCaller& end_callback) noexcept
        = 0;
    [[nodiscard]] virtual std::shared_ptr<Builder> build_shadow_caster_directional(
        const std::string& name,
        std::size_t shadow_map_resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        const core::sync::EndCaller& end_callback) noexcept
        = 0;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_point(
    //    const std::string& name,
    //    const core::sync::EndCaller& end_callback) noexcept;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_shadow_caster_point(
    //    const std::string& name,
    //    std::size_t shadow_map_resolution,
    //    const core::sync::EndCaller& end_callback) noexcept;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_spot(
    //    const std::string& name,
    //    const core::sync::EndCaller& end_callback) noexcept;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_shadow_caster_spot(
    //    const std::string& name,
    //    std::size_t shadow_map_resolution,
    //    const core::sync::EndCaller& end_callback) noexcept;
};
}
#endif
