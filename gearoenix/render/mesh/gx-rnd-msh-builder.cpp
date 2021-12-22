#include "gx-rnd-msh-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-msh-mesh.hpp"

gearoenix::render::mesh::Builder::Builder(
    render::engine::Engine& e,
    const std::string& name,
    math::Aabb3&& occlusion_box) noexcept
    : entity_builder(e.get_world()->create_shared_builder())
    , name(name)
    , occlusion_box(std::move(occlusion_box))
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(physics::Transformation());
}

void gearoenix::render::mesh::Builder::set_material_type_index(const std::type_index& in_material_type) noexcept
{
    auto& builder = entity_builder->get_builder();
    if (const auto* msh = builder.get_component<Mesh>(); nullptr != msh)
        GX_LOG_F("Material already exist in mesh entity: " << *builder.get_name())
    builder.add_component(Mesh(occlusion_box, in_material_type));
}

gearoenix::render::mesh::Builder::~Builder() noexcept = default;

void gearoenix::render::mesh::Builder::set_material(const material::Pbr& mat) noexcept
{
    set_material_type_index(std::type_index(typeid(material::Pbr)));
    auto msh = entity_builder->get_builder().get_component<Mesh>();
    auto& bts = msh->bound_textures;
    bts[texture::BindingPoint::Albedo] = mat.get_albedo();
    bts[texture::BindingPoint::NormalMetallic] = mat.get_normal__metallic();
    bts[texture::BindingPoint::EmissionRoughness] = mat.get_emission__roughness();
}