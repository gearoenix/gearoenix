#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../render/reflection/gx-rnd-rfl-manager.hpp"
#include "../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "gx-gl-types.hpp"
#include <array>
#include <boost/container/static_vector.hpp>

namespace gearoenix::gl {
struct Engine;
struct Target;
struct TextureCube;

struct ReflectionProbe {
    constexpr static auto max_count = render::reflection::Probe::max_count;
    constexpr static auto object_type_index = gearoenix_gl_reflection_probe_type_index;
    constexpr static auto all_parent_object_type_indices = core::Object::all_parent_object_type_indices;
    constexpr static auto immediate_parent_object_type_indices = core::Object::immediate_parent_object_type_indices;

    GX_GET_CREF_PRT(std::shared_ptr<TextureCube>, gl_irradiance);
    GX_GET_CREF_PRT(std::shared_ptr<TextureCube>, gl_radiance);
    GX_GET_VAL_PRT(uint, gl_irradiance_v, static_cast<uint>(-1));
    GX_GET_VAL_PRT(uint, gl_radiance_v, static_cast<uint>(-1));

public:
    virtual ~ReflectionProbe();
};

struct BakedReflection final : render::reflection::Baked, ReflectionProbe {
    constexpr static auto max_count = Baked::max_count;
    constexpr static auto object_type_index = gearoenix_gl_reflection_baked_type_index;
    constexpr static std::array all_parent_object_type_indices { Baked::object_type_index, ReflectionProbe::object_type_index, Probe::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Baked::object_type_index, ReflectionProbe::object_type_index };

    BakedReflection(std::string&& name, std::shared_ptr<TextureCube>&& irr, std::shared_ptr<TextureCube>&& rad, const math::Aabb3<double>& include_box);
    ~BakedReflection() override;
};

struct RuntimeReflection final : render::reflection::Runtime, ReflectionProbe {
    typedef std::array<std::shared_ptr<Target>, 6> GlCubeTarget;
    typedef std::array<boost::container::static_vector<std::shared_ptr<Target>, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6> GlMippedCubeTarget;
    typedef std::array<uint, 6> GlCubeTargetV;
    typedef std::array<boost::container::static_vector<uint, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6> GlMippedCubeTargetV;

    constexpr static auto max_count = Runtime::max_count;
    constexpr static auto object_type_index = gearoenix_gl_reflection_runtime_type_index;
    constexpr static std::array all_parent_object_type_indices { Runtime::object_type_index, ReflectionProbe::object_type_index, Probe::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Runtime::object_type_index, ReflectionProbe::object_type_index };

    GX_GET_CREF_PRV(std::shared_ptr<TextureCube>, gl_environment);
    GX_GET_CREF_PRV(GlCubeTarget, gl_environment_targets);
    GX_GET_CREF_PRV(GlCubeTarget, gl_irradiance_targets);
    GX_GET_CREF_PRV(GlMippedCubeTarget, gl_radiance_targets);

    GX_GET_VAL_PRV(uint, gl_environment_v, static_cast<uint>(-1));
    GX_GET_CREF_PRV(GlCubeTargetV, gl_environment_targets_v);
    GX_GET_CREF_PRV(GlCubeTargetV, gl_irradiance_targets_v);
    GX_GET_CREF_PRV(GlMippedCubeTargetV, gl_radiance_targets_v);

    void initialise_gl();

public:
    RuntimeReflection(
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name);
    static void construct(
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCallerShared<RuntimeReflection>&& end_callback);
    ~RuntimeReflection() override;
};

struct ReflectionManager final : render::reflection::Manager, core::Singleton<ReflectionManager> {
private:
    [[nodiscard]] core::ecs::EntityPtr build_baked(
        std::string&& name,
        core::ecs::Entity* parent,
        std::shared_ptr<render::texture::TextureCube>&& irradiance,
        std::shared_ptr<render::texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box) override;

    void build_runtime(
        std::string&& name,
        core::ecs::Entity* parent,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;

public:
    ReflectionManager();
    ~ReflectionManager() override;
};
}

#endif
