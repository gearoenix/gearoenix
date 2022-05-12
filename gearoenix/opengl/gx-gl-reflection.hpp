#ifndef GEAROENIX_GL_REFLECTION_HPP
#define GEAROENIX_GL_REFLECTION_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/reflection/gx-rnd-rfl-builder.hpp"
#include "../render/reflection/gx-rnd-rfl-manager.hpp"
#include "gx-gl-types.hpp"
#include <array>
#include <boost/container/static_vector.hpp>

namespace gearoenix::gl {
struct Engine;
struct Target;
struct TextureCube;

struct Reflection final : public core::ecs::Component {
    friend struct ReflectionBuilder;
    friend struct ReflectionManager;

    GX_GET_CREF_PRV(std::shared_ptr<TextureCube>, irradiance)
    GX_GET_CREF_PRV(std::shared_ptr<TextureCube>, radiance)
    GX_GET_VAL_PRV(uint, irradiance_v, static_cast<uint>(-1))
    GX_GET_VAL_PRV(uint, radiance_v, static_cast<uint>(-1))

    Reflection() noexcept;
    ~Reflection() noexcept final;
    Reflection(Reflection&&) noexcept;
};

struct ReflectionRuntime final : public core::ecs::Component {
    friend struct ReflectionBuilder;
    friend struct ReflectionManager;

    GX_GET_CREF_PRV(std::shared_ptr<TextureCube>, environment)
    GX_GET_CREF_PRV(std::array<std::shared_ptr<Target> GX_COMMA 6>, environment_targets)
    GX_GET_CREF_PRV(std::array<std::shared_ptr<Target> GX_COMMA 6>, irradiance_targets)
    GX_GET_CREF_PRV(std::array<boost::container::static_vector<std::shared_ptr<Target> GX_COMMA GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT> GX_COMMA 6>, radiance_targets)

    GX_GET_VAL_PRV(uint, environment_v, {})
    GX_GET_VAL_PRV(std::array<uint GX_COMMA 6>, environment_targets_v, {})
    GX_GET_VAL_PRV(std::array<uint GX_COMMA 6>, irradiance_targets_v, {})
    GX_GET_VAL_PRV(std::array<boost::container::static_vector<uint GX_COMMA GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT> GX_COMMA 6>, radiance_targets_v, {})

    ReflectionRuntime() noexcept;
    ~ReflectionRuntime() noexcept final;
    ReflectionRuntime(ReflectionRuntime&&) noexcept;
};

struct ReflectionBuilder final : public render::reflection::Builder {
    friend struct ReflectionManager;

private:
    ReflectionBuilder(
        Engine& e,
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::size_t environment_resolution,
        std::size_t irradiance_resolution,
        std::size_t radiance_resolution,
        std::size_t radiance_mipmap_levels,
        const core::sync::EndCallerIgnored& end_callback) noexcept;

public:
    ~ReflectionBuilder() noexcept final;
};

struct ReflectionManager final : public render::reflection::Manager {
    Engine& eng;

private:
    [[nodiscard]] std::shared_ptr<render::reflection::Builder> build_runtime(
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::size_t environment_resolution,
        std::size_t irradiance_resolution,
        std::size_t radiance_resolution,
        std::size_t radiance_mipmap_levels,
        const core::sync::EndCallerIgnored& end_callback) noexcept final;

public:
    explicit ReflectionManager(Engine& e) noexcept;
    ~ReflectionManager() noexcept final;
};
}

#endif
#endif
