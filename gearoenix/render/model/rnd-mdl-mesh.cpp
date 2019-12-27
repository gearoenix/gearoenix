#include "rnd-mdl-mesh.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include <utility>

gearoenix::render::model::Mesh::Mesh(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    core::sync::EndCaller<mesh::Mesh> call([c](const std::shared_ptr<mesh::Mesh>&) {});
    msh = e->get_system_application()->get_asset_manager()->get_mesh_manager()->get_gx3d(f->read<core::Id>(), call);
    mat = std::shared_ptr<material::Material>(material::Material::read(f, e, core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {})));
}

gearoenix::render::model::Mesh::Mesh(std::shared_ptr<mesh::Mesh> msh, std::shared_ptr<material::Material> mat) noexcept
    : msh(std::move(msh))
    , mat(std::move(mat))
{
}

gearoenix::render::model::Mesh::~Mesh() noexcept
{
    mat = nullptr;
    msh = nullptr;
}

void gearoenix::render::model::Mesh::update() noexcept
{
    mat->update();
}
