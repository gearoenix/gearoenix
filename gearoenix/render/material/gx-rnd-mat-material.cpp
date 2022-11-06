#include "gx-rnd-mat-material.hpp"

gearoenix::render::material::Material::Material(const Id id) noexcept
    : id(id)
{
}

gearoenix::render::material::Material::~Material() noexcept = default;
