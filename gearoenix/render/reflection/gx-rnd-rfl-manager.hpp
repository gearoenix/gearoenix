#ifndef GEAROENIX_RENDER_MANAGER_MANAGER_HPP
#define GEAROENIX_RENDER_MANAGER_MANAGER_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include <string>

namespace gearoenix::platform::stream {
struct Path;
}
namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Builder;
struct Manager {
protected:
    engine::Engine& e;

public:
    explicit Manager(engine::Engine& e);
    virtual ~Manager();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    void build_baked(
        const std::string& name,
        const platform::stream::Path& path,
        core::job::EndCallerShared<Builder>&& c,
        core::job::EndCaller<>&& entity_end_callback);

    [[nodiscard]] virtual std::shared_ptr<Builder> build_baked(
        const std::string& name,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box,
        core::job::EndCaller<>&& end_callback)
        = 0;

    virtual void build_runtime(
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<>&& entity_end_callback,
        core::job::EndCallerShared<Builder>&& probe_end_callback)
        = 0;

    virtual void update();
};
}
#endif
