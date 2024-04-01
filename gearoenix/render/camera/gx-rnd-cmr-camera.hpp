#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../../math/gx-math-ray.hpp"
#include "gx-rnd-cmr-bloom-data.hpp"
#include "gx-rnd-cmr-colour-tuning.hpp"
#include "gx-rnd-cmr-exposure.hpp"
#include "gx-rnd-cmr-projection.hpp"
#include <array>
#include <optional>

#ifdef projection
#undef projection
#endif
#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::texture {
struct Target;
}

namespace gearoenix::render::camera {
struct Camera : core::ecs::Component {
    friend struct Builder;
    friend struct Manager;

    static constexpr std::size_t MAX_COUNT = 16;

    enum struct Usage : std::uint8_t {
        ReflectionProbe = 5,
        Shadow = 11,
        Main = 23,
    };

    engine::Engine& e;
    GX_GET_CREF_PRT(math::Mat4x4<float>, view);
    GX_GET_CREF_PRT(math::Mat4x4<float>, projection);
    GX_GET_CREF_PRT(math::Mat4x4<float>, view_projection);
    GX_GET_CREF_PRT(math::Vec4<float>, starting_clip_ending_clip);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Target>, customised_target);
    // GX_GET_CREF_PRT(std::shared_ptr<texture::Target>, second_target); // todo move the logic of the render target to render graph, in here we only have to have customised_target
    GX_GET_VAL_PRT(bool, has_customised_target_aspect_ratio, false);
    GX_GET_VAL_PRT(float, target_aspect_ratio, 1.7f);
    GX_GETSET_VAL_PRT(core::ecs::entity_id_t, parent_entity_id, 0); // It can be light or reflection probe or any other owner entity
    GX_GETSET_VAL_PRT(core::ecs::entity_id_t, scene_id, 0);
    GX_GETSET_VAL_PRT(std::uint64_t, flag, 1);
    GX_GET_VAL_PRT(float, far, 100.0f);
    GX_GET_VAL_PRT(float, near, 1.0f);
    GX_GET_REF_PRT(ColourTuning, colour_tuning);
    GX_GET_VAL_PRT(ProjectionData, projection_data, PerspectiveProjectionData {});
    GX_GETSET_VAL_PRT(double, layer, 0.0);
    GX_GETSET_VAL_PRT(Usage, usage, Usage::Main);
    GX_GET_VAL_PRT(bool, debug_enabled, false);
    GX_GET_REFC_PRT(math::Vec4<float>, debug_colour);
    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, debug_mesh);
    GX_GET_CREF_PRT(std::optional<BloomData>, bloom_data);
    GX_GET_CREF_PRT(Exposure, exposure);
    GX_GET_VAL_PRT(std::size_t, resolution_cfg_listener, 0);
    GX_GET_CREF_PRV(std::weak_ptr<Camera>, camera_self);

protected:
    Camera(
        engine::Engine& e,
        std::type_index final_type,
        const std::string& name,
        std::size_t resolution_cfg_listener,
        std::shared_ptr<texture::Target>&& customised_target = nullptr,
        ProjectionData projection_type = PerspectiveProjectionData {},
        float near = 1.0f,
        float far = 100.0f,
        bool has_bloom = true);
    void set_component_self(const std::shared_ptr<Component>&) override;

public:
    ~Camera() override;
    void generate_frustum_points(
        const math::Vec3<double>& location,
        const math::Vec3<double>& x,
        const math::Vec3<double>& y,
        const math::Vec3<double>& z,
        std::array<math::Vec3<double>, 8>& points) const;
    void set_view(const math::Mat4x4<float>& view);
    void set_customised_target_aspect_ratio(float target_aspect_ratio);
    void set_projection_data(ProjectionData);
    void update_projection();
    void set_near(float);
    void set_far(float);
    void show_debug_gui() final;
    void enable_debug_mesh();
    void disable_debug_mesh();
    void set_has_customised_target_aspect_ratio(bool b);
    [[nodiscard]] math::Ray3<double> generate_ray(const physics::Transformation& transform, const math::Vec2<double>& normalised_point) const;
    virtual void update_target_aspect_ratio();
    virtual void set_target_aspect_ratio(float ratio);
    virtual void set_customised_target(std::shared_ptr<texture::Target>&&);
    virtual void create_debug_mesh();
    void disable_bloom();
};
}
#endif
