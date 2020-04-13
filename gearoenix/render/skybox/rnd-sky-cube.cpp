#include "rnd-sky-cube.hpp"
#include "../material/rnd-mat-skybox-cube.hpp"

gearoenix::render::skybox::Cube::Cube(
    const core::Id my_id,
    system::stream::Stream* const s,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Skybox(Type::Cube, my_id, s, e, c)
    , mat_cube(new material::SkyboxCube(s, e, c))
{
    mat = mat_cube;
}

gearoenix::render::skybox::Cube::Cube(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Skybox(Type::Cube, my_id, e, c)
    , mat_cube(new material::SkyboxCube(e, c))
{
    mat = mat_cube;
}

gearoenix::render::skybox::Cube::~Cube() noexcept = default;
