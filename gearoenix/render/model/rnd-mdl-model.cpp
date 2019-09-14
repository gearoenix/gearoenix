#include "rnd-mdl-model.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-static.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../widget/rnd-wdg-widget.hpp"

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    const Type t,
    physics::Transformation* const tran,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::MODEL)
    , model_type(t)
    , transformation(tran)
    , uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(Uniform)), e))
    , occlusion_sphere(math::Vec3(0.0f, 0.0f, 0.0f), 1.0f)
    , e(e)
{
    uniform.m.read(f);
    const auto meshes_count = f->read<core::Count>();
    for (core::Count i = 0; i < meshes_count; ++i) {
        add_mesh(std::make_shared<Mesh>(f, e, c));
    }
}

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    const Type t,
    physics::Transformation* const transformation,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
    : core::asset::Asset(my_id, core::asset::Type::MODEL)
    , model_type(t)
    , transformation(transformation)
    , uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(Uniform)), e))
    , occlusion_sphere(math::Vec3(0.0f, 0.0f, 0.0f), 1.0f)
    , e(e)
{
}

gearoenix::render::model::Model::~Model() noexcept
{
	meshes.clear();
	children.clear();
}

void gearoenix::render::model::Model::update() noexcept
{
    uniform_buffers->update(&uniform);
    for (const auto& msh : meshes)
        msh.second->update_uniform();
    for (const auto& ch : children)
        ch.second->update();
}

void gearoenix::render::model::Model::add_mesh(const std::shared_ptr<Mesh>& m) noexcept
{
    meshes[m->get_mesh()->get_asset_id()] = m;
    occlusion_sphere.insert(m->get_mesh()->get_radius());
    if (m->get_material()->get_is_shadow_caster()) {
        shadowing = gearoenix::core::State::Set;
    }
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

const gearoenix::math::Mat4x4& gearoenix::render::model::Model::get_model_matrix() const noexcept
{
    return uniform.m;
}

void gearoenix::render::model::Model::set_enability(const core::State s) noexcept
{
    enability = s;
    for (auto& c : children) {
        c.second->set_enability(s);
    }
    if (nullptr != scene) {
        scene->set_models_changed(true);
    }
}

void gearoenix::render::model::Model::set_scene(scene::Scene* const s) noexcept
{
    scene = s;
}