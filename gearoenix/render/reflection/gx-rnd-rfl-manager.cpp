#include "gx-rnd-rfl-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-rfl-builder.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::reflection::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::reflection::Builder> gearoenix::render::reflection::Manager::build_runtime(
    const std::string& name,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::size_t environment_resolution,
    const std::size_t irradiance_resolution,
    const std::size_t radiance_resolution,
    const std::size_t radiance_mipmap_levels,
    const core::sync::EndCallerIgnored& end_callback) noexcept
{
    return std::make_shared<Builder>(
        e,
        name,
        receive_box,
        exclude_box,
        include_box,
        environment_resolution,
        irradiance_resolution,
        radiance_resolution,
        radiance_mipmap_levels,
        end_callback);
}
