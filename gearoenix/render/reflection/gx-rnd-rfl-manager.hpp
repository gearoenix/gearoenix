#ifndef GEAROENIX_RENDER_MANAGER_MANAGER_HPP
#define GEAROENIX_RENDER_MANAGER_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
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
    explicit Manager(engine::Engine& e) noexcept;
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    virtual ~Manager() noexcept;

    [[nodiscard]] std::shared_ptr<Builder> build_baked(
        const std::string& name,
        const platform::stream::Path& path,
        const core::sync::EndCallerIgnored& c) noexcept;

    [[nodiscard]] virtual std::shared_ptr<Builder> build_baked(
        const std::string& name,
        const std::shared_ptr<texture::TextureCube>& irradiance,
        const std::shared_ptr<texture::TextureCube>& radiance,
        const math::Aabb3<double>& include_box,
        const core::sync::EndCallerIgnored& end_callback) noexcept = 0;

    [[nodiscard]] virtual std::shared_ptr<Builder> build_runtime(
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::size_t environment_resolution,
        std::size_t irradiance_resolution,
        std::size_t radiance_resolution,
        const core::sync::EndCallerIgnored& end_callback) noexcept = 0;

    virtual void update() noexcept;
};
}
#endif
