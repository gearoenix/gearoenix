#include "gx-rnd-mat-material.hpp"
#include <utility>

gearoenix::render::material::Material::Material(std::string name, const Id id, const bool need_model_view_projection_matrix)
    : id(id)
    , name(std::move(name))
    , need_model_view_projection_matrix(need_model_view_projection_matrix)
{
}

gearoenix::render::material::Material::~Material() = default;

void gearoenix::render::material::Material::set_albedo(std::shared_ptr<texture::Texture2D>&&)
{
    GX_UNIMPLEMENTED;
}