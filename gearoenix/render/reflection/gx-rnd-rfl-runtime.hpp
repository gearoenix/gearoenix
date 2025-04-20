#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include "gx-rnd-rfl-probe.hpp"
#include <array>
#include <boost/container/static_vector.hpp>
#include <functional>
#include <limits>
#include <memory>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::texture {
struct Target;
struct Texture2D;
}

namespace gearoenix::render::reflection {
struct Runtime : Probe {
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

    struct CameraData final {
        std::shared_ptr<camera::Camera> cmr;
        std::shared_ptr<physics::Transformation> trn;
    };

    typedef std::array<CameraData, 6> CubeCamera;
    typedef std::array<std::shared_ptr<texture::Target>, 6> CubeTarget;
    typedef std::array<boost::container::static_vector<std::shared_ptr<texture::Target>, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6> MippedCubeTarget;
    typedef boost::container::static_vector<double, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT> MippedRoughness;

    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_render_reflection_runtime_type_index;
    constexpr static std::array all_parent_object_type_indices { Probe::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Probe::object_type_index };

    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, environment);
    GX_GET_CREF_PRT(CubeCamera, cameras);
    GX_GET_CREF_PRT(CubeTarget, environment_targets);
    GX_GET_CREF_PRT(CubeTarget, irradiance_targets);
    GX_GET_CREF_PRT(MippedCubeTarget, radiance_targets);
    GX_GET_CREF_PRT(MippedRoughness, roughnesses);
    GX_GET_VAL_PRT(State, state, State::Uninitialized);
    GX_GET_VAL_PRT(std::uint32_t, state_environment_face, 0);
    GX_GET_VAL_PRT(std::uint32_t, state_irradiance_face, 0);
    GX_GET_VAL_PRT(std::uint32_t, state_radiance_face, 0);
    GX_GET_VAL_PRT(std::uint32_t, state_radiance_level, 0);
    GX_GET_VAL_PRT(std::uint32_t, state_resting_frame, 0);
    GX_GETSET_VAL_PRT(std::uint64_t, resting_frames_count, std::numeric_limits<std::uint64_t>::max());
    GX_GET_CREF_PRT(std::function<void()>, on_rendered);
    GX_GETSET_VAL_PRT(bool, pending_to_start, false);
    GX_GET_CREF_PRT(math::Aabb3<double>, receive_box);
    GX_GET_CREF_PRT(math::Aabb3<double>, exclude_box);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, environment_depth);

    Runtime(
        core::object_type_index_t final_component_type_index,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name);
    void set_runtime_reflection_self(
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<>&& end_callback);

public:
    ~Runtime() override;
    void set_location(const math::Vec3<double>& p);
    void update_state();
    void set_on_rendered(std::function<void()>&&);
    void export_baked(const std::shared_ptr<platform::stream::Stream>& s, core::job::EndCaller<>&& end_callback) const;
};
}