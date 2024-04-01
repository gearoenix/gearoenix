#include "gx-rnd-mat-material.hpp"
#include <utility>

gearoenix::render::material::Material::Material(engine::Engine& e, std::string name, const Id id)
    : e(e)
    , id(id)
    , name(std::move(name))
{
}

gearoenix::render::material::Material::~Material() = default;

void gearoenix::render::material::Material::set_albedo(std::shared_ptr<texture::Texture2D>&&)
{
    GX_UNIMPLEMENTED;
}