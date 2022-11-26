#include "gx-rnd-mat-material.hpp"
#include <utility>

gearoenix::render::material::Material::Material(std::string name, const Id id) noexcept
    : id(id)
    , name(std::move(name))
{
}

gearoenix::render::material::Material::~Material() noexcept = default;

void gearoenix::render::material::Material::set_albedo(const std::shared_ptr<texture::Texture2D>&) noexcept
{
    GX_UNIMPLEMENTED;
}