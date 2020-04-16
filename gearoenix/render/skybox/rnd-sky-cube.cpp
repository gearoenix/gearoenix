#include "rnd-sky-cube.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-skybox-cube.hpp"
#include "../reflection/rnd-rfl-baked.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::skybox::Cube::Cube(
    const core::Id my_id,
    system::stream::Stream* const s,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Skybox(Type::Cube, my_id, s, e, c)
    , mat_cube(new material::SkyboxCube(s, e, c))
{
    mat = mat_cube;
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    core::sync::EndCaller<texture::Texture> txt_call([c](const std::shared_ptr<texture::Texture>&) {});
    auto irr = std::dynamic_pointer_cast<texture::TextureCube>(txt_mgr->read_gx3d(s, txt_call));
    auto rad = std::dynamic_pointer_cast<texture::TextureCube>(txt_mgr->read_gx3d(s, txt_call));
    baked_reflection = std::make_shared<reflection::Baked>(std::move(irr), std::move(rad));
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
