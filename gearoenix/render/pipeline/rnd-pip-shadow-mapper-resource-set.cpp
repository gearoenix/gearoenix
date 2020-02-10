#include "rnd-pip-shadow-mapper-resource-set.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../material/rnd-mat-pbr.hpp"
#include "../material/rnd-mat-unlit.hpp"
#include "rnd-pip-shadow-mapper.hpp"

gearoenix::render::pipeline::ShadowMapperResourceSet::ShadowMapperResourceSet(
    std::shared_ptr<ShadowMapper const> pip) noexcept
    : ResourceSet(std::move(pip))
{
}

gearoenix::render::pipeline::ShadowMapperResourceSet::~ShadowMapperResourceSet() noexcept = default;

void gearoenix::render::pipeline::ShadowMapperResourceSet::set_mesh(const mesh::Mesh* const m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::ShadowMapperResourceSet::set_material(const material::Material* m) noexcept
{
    switch (m->get_material_type()) {
    case material::Type::Pbr:
        color = reinterpret_cast<const material::Pbr*>(m)->get_color_texture().get();
        break;
    case material::Type::Unlit:
        color = reinterpret_cast<const material::Unlit*>(m)->get_color_texture().get();
        break;
    default:
        GXUNEXPECTED
    }
}

void gearoenix::render::pipeline::ShadowMapperResourceSet::set_node_uniform_buffer(const buffer::Uniform* const b) noexcept
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::ShadowMapperResourceSet::clean() noexcept
{
    node_uniform_buffer = nullptr;
    msh = nullptr;
    color = nullptr;
}