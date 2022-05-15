#ifndef GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#define GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include <array>
#include <boost/container/static_vector.hpp>
#include <functional>
#include <limits>
#include <memory>
#include <optional>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Target;
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Builder;
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
    GX_GET_VAL_PRV(core::ecs::Entity::id_t, scene_id, 0)
    GX_GET_RRF_PRV(engine::Engine, e)
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, environment)
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, radiance)
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, irradiance)
    GX_GET_CREF_PRV(std::array<core::ecs::Entity::id_t GX_COMMA 6>, cameras)
    GX_GET_CREF_PRV(std::array<std::shared_ptr<texture::Target> GX_COMMA 6>, environment_targets)
    GX_GET_CREF_PRV(std::array<std::shared_ptr<texture::Target> GX_COMMA 6>, irradiance_targets)
    GX_GET_CREF_PRV(std::array<boost::container::static_vector<std::shared_ptr<texture::Target> GX_COMMA GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT> GX_COMMA 6>, radiance_targets)
    GX_GET_CREF_PRV(boost::container::static_vector<double GX_COMMA GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, roughnesses)
    GX_GET_VAL_PRV(State, state, State::Uninitialized)
    GX_GET_VAL_PRV(std::size_t, state_environment_face, 0)
    GX_GET_VAL_PRV(std::size_t, state_irradiance_face, 0)
    GX_GET_VAL_PRV(std::size_t, state_radiance_face, 0)
    GX_GET_VAL_PRV(std::size_t, state_radiance_level, 0)
    GX_GET_VAL_PRV(std::size_t, state_resting_frame, 0)
    GX_GETSET_VAL_PRV(std::size_t, resting_frames_count, std::numeric_limits<std::size_t>::max())
    GX_GETSET_CREF_PRV(std::optional<std::function<void()>>, on_rendered)
    GX_GETSET_VAL_PRV(bool, pending_to_start, false)
    GX_GET_CREF_PRV(math::Aabb3<double>, receive_box)
    GX_GET_CREF_PRV(math::Aabb3<double>, exclude_box)
    GX_GET_CREF_PRV(math::Aabb3<double>, include_box)

public:
    Runtime(
        engine::Engine& e,
        Builder& builder,
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::size_t environment_resolution,
        std::size_t irradiance_resolution,
        std::size_t radiance_resolution,
        const core::sync::EndCallerIgnored& end_callback) noexcept;
    ~Runtime() noexcept final;
    Runtime(Runtime&&) noexcept;
    void set_location(const math::Vec3<double>& p) noexcept;
    void update_state() noexcept;
    /// NOTE: This function must be called when the Component and other related entities (e.g. cameras) are completely placed in the world
    void set_scene_id(core::ecs::Entity::id_t) noexcept;
};
}
#endif