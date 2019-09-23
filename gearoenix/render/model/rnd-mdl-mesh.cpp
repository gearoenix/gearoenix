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
    mat = std::make_shared<material::Material>(f, e, core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {}));
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

const std::shared_ptr<gearoenix::render::mesh::Mesh>& gearoenix::render::model::Mesh::get_mesh() const noexcept
{
    return msh;
}

const std::shared_ptr<gearoenix::render::material::Material>& gearoenix::render::model::Mesh::get_material() const noexcept
{
    return mat;
}

void gearoenix::render::model::Mesh::update_uniform() noexcept
{
    mat->update_uniform();
}
