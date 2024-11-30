#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../render/reflection/gx-rnd-rfl-builder.hpp"
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
    constexpr static std::uint32_t MAX_COUNT = 0;
    constexpr static core::ecs::Component::TypeIndex TYPE_INDEX = 21;
    constexpr static core::ecs::Component::TypeIndexSet ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::Component::TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_CREF_PRT(std::shared_ptr<TextureCube>, gl_irradiance);
    GX_GET_CREF_PRT(std::shared_ptr<TextureCube>, gl_radiance);
    GX_GET_VAL_PRT(uint, gl_irradiance_v, static_cast<uint>(-1));
    GX_GET_VAL_PRT(uint, gl_radiance_v, static_cast<uint>(-1));

public:
    virtual ~ReflectionProbe();
};

struct BakedReflection final : render::reflection::Baked, ReflectionProbe {
    constexpr static auto MAX_COUNT = Baked::MAX_COUNT;
    constexpr static TypeIndex TYPE_INDEX = 19;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES { Baked::TYPE_INDEX, ReflectionProbe::TYPE_INDEX, Probe::TYPE_INDEX };
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES { Baked::TYPE_INDEX, ReflectionProbe::TYPE_INDEX };

public:
    BakedReflection(
        std::string&& name,
        Engine& e,
        std::shared_ptr<TextureCube>&& irr,
        std::shared_ptr<TextureCube>&& rad,
        const math::Aabb3<double>& include_box,
        core::ecs::entity_id_t entity_id);
    ~BakedReflection() override;
};

struct RuntimeReflection final : render::reflection::Runtime, ReflectionProbe {
    typedef std::array<std::shared_ptr<Target>, 6> GlCubeTarget;
    typedef std::array<boost::container::static_vector<std::shared_ptr<Target>, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6> GlMippedCubeTarget;
    typedef std::array<uint, 6> GlCubeTargetV;
    typedef std::array<boost::container::static_vector<uint, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6> GlMippedCubeTargetV;

    constexpr static auto MAX_COUNT = Runtime::MAX_COUNT;
    constexpr static TypeIndex TYPE_INDEX = 15;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES { Runtime::TYPE_INDEX, ReflectionProbe::TYPE_INDEX, Probe::TYPE_INDEX };
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES { Runtime::TYPE_INDEX, ReflectionProbe::TYPE_INDEX };

    GX_GET_CREF_PRV(std::shared_ptr<TextureCube>, gl_environment);
    GX_GET_CREF_PRV(GlCubeTarget, gl_environment_targets);
    GX_GET_CREF_PRV(GlCubeTarget, gl_irradiance_targets);
    GX_GET_CREF_PRV(GlMippedCubeTarget, gl_radiance_targets);

    GX_GET_VAL_PRV(uint, gl_environment_v, static_cast<uint>(-1));
    GX_GET_CREF_PRV(GlCubeTargetV, gl_environment_targets_v);
    GX_GET_CREF_PRV(GlCubeTargetV, gl_irradiance_targets_v);
    GX_GET_CREF_PRV(GlMippedCubeTargetV, gl_radiance_targets_v);

public:
    RuntimeReflection(
        Engine& e,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    static void construct(
        Engine& e,
        const std::shared_ptr<render::reflection::Builder>& builder,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::string&& name,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::ecs::entity_id_t entity_id,
        core::job::EndCallerShared<RuntimeReflection>&& end_callback);
    ~RuntimeReflection() override;
};

struct ReflectionBuilder final : render::reflection::Builder {
private:
    ReflectionBuilder(
        Engine& e,
        const std::string& name,
        physics::Transformation* parent_transform,
        core::job::EndCaller<>&& end_callback);

public:
    ReflectionBuilder(
        Engine& e,
        const std::string& name,
        physics::Transformation* parent_transform,
        const math::Aabb3<double>& include_box,
        std::shared_ptr<render::texture::TextureCube>&& irradiance_texture,
        std::shared_ptr<render::texture::TextureCube>&& radiance_texture,
        core::job::EndCaller<>&& end_callback);
    static void construct_runtime(
        Engine& e,
        const std::string& name,
        physics::Transformation* parent_transform,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<>&& entity_end_callback,
        core::job::EndCallerShared<ReflectionBuilder>&& probe_end_callback);
    ~ReflectionBuilder() override;
};

struct ReflectionManager final : render::reflection::Manager {
    Engine& eng;

private:
    [[nodiscard]] std::shared_ptr<render::reflection::Builder> build_baked(
        const std::string& name,
        physics::Transformation* parent_transform,
        std::shared_ptr<render::texture::TextureCube>&& irradiance,
        std::shared_ptr<render::texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box,
        core::job::EndCaller<>&& end_callback) override;

    void build_runtime(
        const std::string& name,
        physics::Transformation* parent_transform,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<>&& entity_end_callback,
        core::job::EndCallerShared<render::reflection::Builder>&& probe_end_callback) override;

public:
    explicit ReflectionManager(Engine& e);
    ~ReflectionManager() override;
};
}

#endif
