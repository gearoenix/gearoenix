#include "gx-rnd-pip-radiance-convoluter-resource-set.hpp"
#include "gx-rnd-pip-radiance-convoluter.hpp"

gearoenix::render::pipeline::RadianceConvoluterResourceSet::RadianceConvoluterResourceSet(std::shared_ptr<RadianceConvoluter const> pip) noexcept
    : ResourceSet(std::move(pip))
{
}

gearoenix::render::pipeline::RadianceConvoluterResourceSet::~RadianceConvoluterResourceSet() noexcept = default;

void gearoenix::render::pipeline::RadianceConvoluterResourceSet::set_mesh(const mesh::Mesh* m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::RadianceConvoluterResourceSet::set_environment(const texture::TextureCube* c) noexcept
{
    environment = c;
}

void gearoenix::render::pipeline::RadianceConvoluterResourceSet::clean() noexcept
{
    msh = nullptr;
    environment = nullptr;
}

void gearoenix::render::pipeline::RadianceConvoluterResourceSet::set_node_uniform_buffer(const buffer::Uniform* const u) noexcept
{
    node_uniform_buffer = u;
}
