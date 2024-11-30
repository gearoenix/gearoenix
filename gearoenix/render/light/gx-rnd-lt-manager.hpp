#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>
#include <string>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::light {
struct Builder;
struct Manager {
    engine::Engine& e;

    explicit Manager(engine::Engine& e);
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    virtual ~Manager();
    [[nodiscard]] virtual std::shared_ptr<Builder> build_directional(
        const std::string& name,
        physics::Transformation* parent_transform,
        core::job::EndCaller<>&& end_callback)
        = 0;
    virtual void build_shadow_caster_directional(
        const std::string& name,
        physics::Transformation* parent_transform,
        std::uint32_t shadow_map_resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        core::job::EndCallerShared<Builder>&& end_callback,
        core::job::EndCaller<>&& entity_end_callback)
        = 0;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_point(
    //    const std::string& name,
    //    const core::job::EndCaller& end_callback) ;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_shadow_caster_point(
    //    const std::string& name,
    //    std::uint64_t shadow_map_resolution,
    //    const core::job::EndCaller& end_callback) ;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_spot(
    //    const std::string& name,
    //    const core::job::EndCaller& end_callback) ;
    //[[nodiscard]] virtual std::shared_ptr<Builder> build_shadow_caster_spot(
    //    const std::string& name,
    //    std::uint64_t shadow_map_resolution,
    //    const core::job::EndCaller& end_callback) ;
};
}