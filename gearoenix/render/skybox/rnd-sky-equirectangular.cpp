#include "rnd-sky-equirectangular.hpp"
#include "../material/rnd-mat-skybox-equirectangular.hpp"

gearoenix::render::skybox::Equirectangular::Equirectangular(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const s,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Skybox(Type::Equirectangular, my_id, std::move(name), s, e, c)
    , mat_equ(new material::SkyboxEquirectangular(s, e, c))
{
    mat = mat_equ;
    GX_UNIMPLEMENTED
}

gearoenix::render::skybox::Equirectangular::Equirectangular(
    const core::Id my_id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Skybox(Type::Equirectangular, my_id, std::move(name), e, c)
    , mat_equ(new material::SkyboxEquirectangular(e, c))
{
    mat = mat_equ;
}

gearoenix::render::skybox::Equirectangular::~Equirectangular() noexcept = default;
