#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include <string>

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Baked;
struct Manager : core::Singleton<Manager> {
    GX_GET_CREF_PRT(std::shared_ptr<Baked>, black);

public:
    Manager();
    ~Manager() override;

    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    void build_baked(
        std::string&& name,
        core::ecs::Entity* parent,
        const platform::stream::Path& path,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback);

    [[nodiscard]] virtual core::ecs::EntityPtr build_baked(
        std::string&& name,
        core::ecs::Entity* parent,
        std::shared_ptr<texture::TextureCube>&& irradiance,
        std::shared_ptr<texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box)
        = 0;

    virtual void build_runtime(
        std::string&& name,
        core::ecs::Entity* parent,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
        = 0;

    virtual void update();
};
}