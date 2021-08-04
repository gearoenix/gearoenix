#include "gx-rnd-msh-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-msh-mesh.hpp"

gearoenix::render::mesh::Builder::Builder(
    render::engine::Engine& e,
    const std::string& name,
    const std::vector<PbrVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
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
    const auto msh = builder.get_component<Mesh>();
    if (msh.has_value()) {
        GX_LOG_F("Material already exist in mesh entity: " << *builder.get_name())
    }
    builder.add_component(Mesh(occlusion_box, in_material_type));
}

gearoenix::render::mesh::Builder::~Builder() noexcept = default;
