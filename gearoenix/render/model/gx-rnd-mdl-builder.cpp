#include "gx-rnd-mdl-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-mdl-model.hpp"

gearoenix::render::model::Builder::Builder(
    render::engine::Engine& e,
    const std::string& name,
    std::shared_ptr<mesh::Mesh>&& _bound_mesh,
    bool is_transformable) noexcept
    : entity_builder(e.get_world()->create_shared_builder())
    , bound_mesh(std::move(_bound_mesh))
    , is_transformable(is_transformable)
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(physics::collider::Aabb3(bound_mesh->box));
    builder.add_component(physics::Transformation());
}

void gearoenix::render::model::Builder::set_material_type_index(const std::type_index& in_material_type) noexcept
{
    auto& builder = entity_builder->get_builder();
    if (const auto* model = builder.get_component<Model>(); nullptr != model)
        GX_LOG_F("Material already exist in mesh entity: " << *builder.get_name());
    builder.add_component(Model(std::move(bound_mesh), in_material_type, is_transformable));
}

gearoenix::render::model::Builder::~Builder() noexcept = default;

void gearoenix::render::model::Builder::set_material(const material::Pbr& mat) noexcept
{
    set_material_type_index(std::type_index(typeid(material::Pbr)));
    auto model = entity_builder->get_builder().get_component<Model>();
    auto& bts = model->bound_textures;
    bts[texture::BindingPoint::Albedo] = mat.get_albedo();
    bts[texture::BindingPoint::Normal] = mat.get_normal();
    bts[texture::BindingPoint::Emission] = mat.get_emission();
    bts[texture::BindingPoint::MetallicRoughness] = mat.get_metallic_roughness();
    bts[texture::BindingPoint::Occlusion] = mat.get_occlusion();
}

gearoenix::physics::Transformation& gearoenix::render::model::Builder::get_transformation() noexcept
{
    return *entity_builder->get_builder().get_component<physics::Transformation>();
}
