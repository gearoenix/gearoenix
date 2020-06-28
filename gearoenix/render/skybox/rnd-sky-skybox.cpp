#include "rnd-sky-skybox.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"

void gearoenix::render::skybox::Skybox::init(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    auto* const ast_mgr = e->get_system_application()->get_asset_manager();
    core::sync::EndCaller<mesh::Mesh> mesh_call([c](const std::shared_ptr<mesh::Mesh>&) {});
    msh = ast_mgr->get_mesh_manager()->create_inward_cube(mesh_call);
}

gearoenix::render::skybox::Skybox::Skybox(
    const Type t,
    const core::Id id,
    std::string name,
    system::stream::Stream* const,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(id, core::asset::Type::Skybox, std::move(name))
    , skybox_type(t)
    , e(e)
{
    init(c);
}

gearoenix::render::skybox::Skybox::Skybox(
    const Type t,
    const core::Id id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(id, core::asset::Type::Skybox, std::move(name))
    , skybox_type(t)
    , e(e)
{
    init(c);
}

void gearoenix::render::skybox::Skybox::update() noexcept
{
    mat->update();
}

gearoenix::render::skybox::Skybox::~Skybox() noexcept = default;