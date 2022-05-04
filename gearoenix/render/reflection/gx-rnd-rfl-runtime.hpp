#ifndef GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#define GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include <functional>
#include <limits>
#include <memory>
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Target;
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Runtime final : public core::ecs::Component {
    enum struct State {
        Uninitialized = 0,
        Started = 1,
        EnvironmentCubeRender = 2,
        EnvironmentCubeMipMap = 3,
        IrradianceFace = 4,
        IrradianceMipMap = 5,
        RadianceFaceLevel = 6,
        Resting = 7,
    };
    GX_GET_RRF_PRV(engine::Engine, e)
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, environment)
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, radiance)
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, irradiance)
    GX_GET_ARRC_PRV(core::ecs::Entity::id_t, cameras, 6)
    GX_GET_CREF_PRV(std::vector<double>, roughnesses)
    GX_GET_VAL_PRV(State, state, State::Uninitialized)
    GX_GET_VAL_PRV(std::size_t, state_environment_face, 0)
    GX_GET_VAL_PRV(std::size_t, state_irradiance_face, 0)
    GX_GET_VAL_PRV(std::size_t, state_radiance_face, 0)
    GX_GET_VAL_PRV(std::size_t, state_radiance_level, 0)
    GX_GET_VAL_PRV(std::size_t, state_resting_frame, 0)
    GX_GETSET_VAL_PRV(std::size_t, resting_frames_count, std::numeric_limits<std::size_t>::max())
    GX_GETSET_CREF_PRV(std::function<void()>, on_rendered)
    GX_GETSET_VAL_PRV(bool, pending_to_start, false)
    GX_GET_CREF_PRV(math::Aabb3<double>, receive_box)
    GX_GET_CREF_PRV(math::Aabb3<double>, exclude_box)
    GX_GET_CREF_PRV(math::Aabb3<double>, include_box)

public:
    Runtime(
        engine::Engine& e,
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::size_t environment_resolution,
        std::size_t irradiance_resolution,
        std::size_t radiance_resolution,
        std::size_t radiance_mipmap_levels,
        const core::sync::EndCallerIgnored& end_callback) noexcept;
    ~Runtime() noexcept final;
    Runtime(Runtime&&) noexcept;
    void set_location(const math::Vec3<double>& p) noexcept;
    void update_state() noexcept;
};
}
#endif