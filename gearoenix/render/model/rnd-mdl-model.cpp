#include "rnd-mdl-model.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/collider/phs-cld-ghost.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "rnd-mdl-manager.hpp"
#include "rnd-mdl-mesh.hpp"
#include "rnd-mdl-transformation.hpp"

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    const Type t,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Model)
    , model_type(t)
    , collider(new physics::collider::Ghost())
    , transformation(new Transformation(this))
    , uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(math::Mat4x4)), e))
    , e(e)
{
    collider->set_parent(this);
    math::Mat4x4 m;
    m.read(f);
    collider->set_model_matrix(m);
    const auto meshes_count = f->read<core::Count>();
    for (core::Count i = 0; i < meshes_count; ++i) {
        add_mesh(std::make_shared<Mesh>(f, e, c));
    }
    std::vector<core::Id> children_ids;
    f->read(children_ids);
    if (children_ids.empty())
        return;
    core::sync::EndCaller<Model> call([c](const std::shared_ptr<Model>&) {});
    Manager* const mgr = e->get_system_application()->get_asset_manager()->get_model_manager();
    for (core::Id c_id : children_ids) {
        children[c_id] = mgr->get_gx3d(c_id, call);
    }
}

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    const Type t,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Model)
    , model_type(t)
    , collider(new physics::collider::Ghost())
    , transformation(new Transformation(this))
    , uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(math::Mat4x4)), e))
    , e(e)
{
    collider->set_parent(this);
}

gearoenix::render::model::Model::~Model() noexcept
{
    collider = nullptr;
    transformation = nullptr;
    meshes.clear();
    children.clear();
}

void gearoenix::render::model::Model::update() noexcept
{
    if (e->get_frame_number_from_start() == latest_frame_update)
        return;
    uniform_buffers->update(collider->get_model_matrix());
    for (const auto& msh : meshes)
        msh.second->update();
    latest_frame_update = e->get_frame_number_from_start();
}

void gearoenix::render::model::Model::add_mesh(const std::shared_ptr<Mesh>& m) noexcept
{
    const material::Material* const mat = m->get_mat().get();
    has_shadow_caster |= mat->get_is_shadow_caster();
    has_transparent |= (mat->get_translucency() == material::TranslucencyMode::Transparent);
    const mesh::Mesh* const msh = m->get_msh().get();
    collider->put_in_box(msh->get_box());
    meshes[msh->get_asset_id()] = m;
}

void gearoenix::render::model::Model::add_child(const std::shared_ptr<Model>& c) noexcept
{
    children[c->get_asset_id()] = c;
    c->parent = this;
}

void gearoenix::render::model::Model::set_collider(std::unique_ptr<physics::collider::Collider> c) noexcept
{
    collider = std::move(c);
    collider->set_parent(this);
}

void gearoenix::render::model::Model::set_enabled(const bool b) noexcept
{
    enabled = b;
    for (auto& c : children) {
        c.second->set_enabled(b);
    }
    if (scene != nullptr) {
        scene->set_models_changed(true);
    }
}

void gearoenix::render::model::Model::set_scene(scene::Scene* const s) noexcept
{
    scene = s;
}