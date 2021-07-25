#include "gx-rnd-mdl-model.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../physics/collider/gx-phs-cld-ghost.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../buffer/gx-rnd-buf-framed-uniform.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../material/gx-rnd-mat-material.hpp"
#include "../material/gx-rnd-mat-pbr.hpp"
#include "../mesh/gx-rnd-msh-builder.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../pipeline/gx-rnd-pip-manager.hpp"
#include "../reflection/gx-rnd-rfl-baked.hpp"
#include "../reflection/gx-rnd-rfl-runtime.hpp"
#include "../scene/gx-rnd-scn-scene.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-mdl-manager.hpp"
#include "gx-rnd-mdl-mesh.hpp"
#include "gx-rnd-mdl-transformation.hpp"

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    std::string name,
    const Type t,
    platform::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Model, std::move(name))
    , model_type(t)
    , collider(new physics::collider::Ghost())
    , transformation(new Transformation(this))
    , uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(math::Mat4x4<float>)), e))
    , e(e)
    , hooked_reflection(nullptr)
{
    collider->set_parent(this);
    math::Mat4x4<float> m;
    m.read(f);
    collider->set_model_matrix(math::Mat4x4<double>(m));
    const auto meshes_count = f->read<core::Count>();
    for (core::Count i = 0; i < meshes_count; ++i) {
        add_mesh(std::make_shared<Mesh>(f, e, c));
    }
    std::vector<core::Id> children_ids;
    f->read(children_ids);
    if (children_ids.empty())
        return;
    core::sync::EndCaller<Model> call([c](const std::shared_ptr<Model>&) {});
    Manager* const mgr = e->get_platform_application()->get_asset_manager()->get_model_manager();
    for (core::Id c_id : children_ids) {
        children[c_id] = mgr->get_gx3d(c_id, call);
    }
}

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    std::string name,
    const Type t,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Model, std::move(name))
    , model_type(t)
    , collider(new physics::collider::Ghost())
    , transformation(new Transformation(this))
    , uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(math::Mat4x4<float>)), e))
    , e(e)
    , hooked_reflection(nullptr)
{
    collider->set_parent(this);
}

gearoenix::render::model::Model::Model(const Model& o) noexcept
    : core::asset::Asset(
        core::asset::Manager::create_id(),
        core::asset::Type::Model,
        o.name + "-clone-" + std::to_string(o.id) + "-" + std::to_string(core::asset::Manager::create_id()))
    , model_type(o.model_type)
    , collider(o.collider->clone())
    , transformation(new Transformation(this))
    , uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(math::Mat4x4<float>)), o.e))
    , has_shadow_caster(o.has_shadow_caster)
    , has_transparent(o.has_transparent)
    , enabled(o.enabled)
    , parent(o.parent)
    , scene(o.scene)
    , e(o.e)
    , latest_frame_update(o.latest_frame_update)
    , hooked_reflection(o.hooked_reflection)
    , colliding_reflection(o.colliding_reflection)
{
    collider->set_parent(this);

    for (const auto& [mesh_id, msh] : o.meshes) {
        meshes[mesh_id] = std::shared_ptr<Mesh>(msh->clone());
    }

    for (const auto& [child_id, mdl] : o.children) {
        children[child_id] = std::shared_ptr<Model>(mdl->clone());
    }
}

void gearoenix::render::model::Model::set_reflection(texture::TextureCube* const irradiance, texture::TextureCube* const radiance) noexcept
{
    for (const auto& msh : meshes) {
        auto* const mat = msh.second->get_mat().get();
        if (material::Type::Pbr == mat->get_material_type()) {
            auto* const pbr = static_cast<material::Pbr*>(mat);
            pbr->set_irradiance(irradiance);
            pbr->set_radiance(radiance);
        }
    }
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
    uniform_buffers->update(math::Mat4x4<float>(collider->get_model_matrix()));
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
    meshes[msh->get_id()] = m;
}

void gearoenix::render::model::Model::add_child(const std::shared_ptr<Model>& c) noexcept
{
    children[c->get_id()] = c;
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

void gearoenix::render::model::Model::set_hooked_reflection(std::shared_ptr<reflection::Reflection> rfl) noexcept
{
    for (const auto& m : children)
        m.second->set_hooked_reflection(rfl);
    //    set_reflection(rfl->get_irradiance().get(), static_cast<reflection::Runtime*>(rfl.get())->get_environment().get());
    set_reflection(rfl->get_irradiance().get(), rfl->get_radiance().get());
    hooked_reflection = std::move(rfl);
}

void gearoenix::render::model::Model::set_colliding_reflection(reflection::Reflection* const rfl) noexcept
{
    colliding_reflection = rfl;
    set_reflection(rfl->get_irradiance().get(), rfl->get_radiance().get());
    for (const auto& m : children)
        m.second->set_colliding_reflection(rfl);
}

void gearoenix::render::model::Model::clear_reflection() noexcept
{
    auto* const rfl = scene->get_default_reflection_probe().get();
    if (rfl == nullptr) {
        core::sync::EndCaller<texture::TextureCube> call([this](const std::shared_ptr<texture::TextureCube>& t) {
            set_reflection(t.get(), t.get());
        });
        (void)e->get_platform_application()->get_asset_manager()->get_texture_manager()->get_cube_zero_3c(call);
        for (const auto& m : children)
            m.second->clear_reflection();
    } else {
        set_colliding_reflection(rfl);
    }
}

void gearoenix::render::model::Model::set_scene(scene::Scene* const s) noexcept
{
    scene = s;
}

gearoenix::render::model::Model* gearoenix::render::model::Model::get_root() noexcept
{
    if (parent == nullptr)
        return this;
    return parent->get_root();
}

const gearoenix::render::model::Model* gearoenix::render::model::Model::get_root() const noexcept
{
    if (parent == nullptr)
        return this;
    return parent->get_root();
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::render::model::Model::clone() const noexcept
{
    GX_UNIMPLEMENTED
}
