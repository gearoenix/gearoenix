#include "rnd-sky-equirectangular.hpp"
#include "../material/rnd-mat-sky-equirectangular.hpp"

gearoenix::render::skybox::Equirectangular::Equirectangular(
    const core::Id my_id,
    system::stream::Stream* const s,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Skybox(Type::Equirectangular, my_id, s, e, c)
    , mat_equ(new material::SkyEquirectangular(s, e, c))
{
    mat = mat_equ;
    GXUNIMPLEMENTED
}

gearoenix::render::skybox::Equirectangular::Equirectangular(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Skybox(Type::Equirectangular, my_id, e, c)
    , mat_equ(new material::SkyEquirectangular(e, c))
{
    mat = mat_equ;
}

gearoenix::render::skybox::Equirectangular::~Equirectangular() noexcept = default;
