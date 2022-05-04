#include "gx-rnd-rfl-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-rfl-baked.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::size_t environment_resolution,
    const std::size_t irradiance_resolution,
    const std::size_t radiance_resolution,
    const std::size_t radiance_mipmap_levels,
    const core::sync::EndCallerIgnored& end_callback) noexcept
    : entity_builder(e.get_world()->create_shared_builder())
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(Runtime(
        e,
        name,
        receive_box,
        exclude_box,
        include_box,
        environment_resolution,
        irradiance_resolution,
        radiance_resolution,
        radiance_mipmap_levels,
        end_callback));
}

gearoenix::render::reflection::Builder::~Builder() noexcept = default;
