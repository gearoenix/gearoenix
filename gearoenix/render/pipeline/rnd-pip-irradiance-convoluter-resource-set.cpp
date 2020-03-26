#include "rnd-pip-irradiance-convoluter-resource-set.hpp"
#include "rnd-pip-irradiance-convoluter.hpp"

gearoenix::render::pipeline::IrradianceConvoluterResourceSet::IrradianceConvoluterResourceSet(std::shared_ptr<IrradianceConvoluter const> pip) noexcept
    : ResourceSet(std::move(pip))
{
}

gearoenix::render::pipeline::IrradianceConvoluterResourceSet::~IrradianceConvoluterResourceSet() noexcept = default;

void gearoenix::render::pipeline::IrradianceConvoluterResourceSet::set_mesh(const mesh::Mesh* m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::IrradianceConvoluterResourceSet::set_environment(const texture::TextureCube* c) noexcept
{
    environment = c;
}

void gearoenix::render::pipeline::IrradianceConvoluterResourceSet::set_node_uniform_buffer(buffer::Uniform* const b) noexcept
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::IrradianceConvoluterResourceSet::clean() noexcept
{
    msh = nullptr;
    environment = nullptr;
}
