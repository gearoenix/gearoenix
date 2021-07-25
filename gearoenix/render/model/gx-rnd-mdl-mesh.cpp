#include "gx-rnd-mdl-mesh.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-material.hpp"
#include "../mesh/gx-rnd-msh-builder.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include <utility>

gearoenix::render::model::Mesh::Mesh(platform::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    core::sync::EndCaller<mesh::Mesh> call([c](const std::shared_ptr<mesh::Mesh>&) {});
    msh = e->get_platform_application()->get_asset_manager()->get_mesh_manager()->get_gx3d(f->read<core::Id>(), call);
    mat = std::shared_ptr<material::Material>(material::Material::read(f, e, c));
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

gearoenix::render::model::Mesh* gearoenix::render::model::Mesh::clone() const noexcept
{
    return new Mesh(msh, std::shared_ptr<material::Material>(mat->clone()));
}
