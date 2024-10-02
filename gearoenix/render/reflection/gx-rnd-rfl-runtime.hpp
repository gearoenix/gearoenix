#ifndef GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#define GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include "gx-rnd-rfl-probe.hpp"
#include <array>
#include <boost/container/static_vector.hpp>
#include <functional>
#include <limits>
#include <memory>

namespace gearoenix::physics {
struct TransformationComponent;
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
struct Builder;
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
        std::shared_ptr<physics::TransformationComponent> trn;
    };

    typedef std::array<CameraData, 6> CubeCamera;
    typedef std::array<std::shared_ptr<texture::Target>, 6> CubeTarget;
    typedef std::array<boost::container::static_vector<std::shared_ptr<texture::Target>, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6> MipedCubeTarget;
    typedef boost::container::static_vector<double, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT> MipedRoughness;

    static constexpr std::size_t MAX_COUNT = 8;

    GX_GET_CREF_PRT(std::shared_ptr<texture::TextureCube>, environment);
    GX_GET_CREF_PRT(CubeCamera, cameras);
    GX_GET_CREF_PRT(CubeTarget, environment_targets);
    GX_GET_CREF_PRT(CubeTarget, irradiance_targets);
    GX_GET_CREF_PRT(MipedCubeTarget, radiance_targets);
    GX_GET_CREF_PRT(MipedRoughness, roughnesses);
    GX_GET_VAL_PRT(State, state, State::Uninitialized);
    GX_GET_VAL_PRT(std::size_t, state_environment_face, 0);
    GX_GET_VAL_PRT(std::size_t, state_irradiance_face, 0);
    GX_GET_VAL_PRT(std::size_t, state_radiance_face, 0);
    GX_GET_VAL_PRT(std::size_t, state_radiance_level, 0);
    GX_GET_VAL_PRT(std::size_t, state_resting_frame, 0);
    GX_GETSET_VAL_PRT(std::uint64_t, resting_frames_count, std::numeric_limits<std::uint64_t>::max());
    GX_GET_CREF_PRT(std::function<void()>, on_rendered);
    GX_GETSET_VAL_PRT(bool, pending_to_start, false);
    GX_GET_CREF_PRT(math::Aabb3<double>, receive_box);
    GX_GET_CREF_PRT(math::Aabb3<double>, exclude_box);
    GX_GET_CREF_PRT(std::weak_ptr<Runtime>, weak_runtime_self);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, environment_depth);

    Runtime(
        engine::Engine& e,
        std::type_index final_component_type_index,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    void set_runtime_reflection_self(
        const std::shared_ptr<Runtime>& runtime_self,
        const std::shared_ptr<Builder>& builder,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<>&& end_callback);

public:
    ~Runtime() override;
    void set_location(const math::Vec3<double>& p);
    void update_state();
    void set_scene_id(core::ecs::entity_id_t) override;
    void set_on_rendered(std::function<void()>&&);
    void export_baked(const std::shared_ptr<platform::stream::Stream>& s, core::job::EndCaller<>&& end_callback) const;
};
}
#endif