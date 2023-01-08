#include "gx-rnd-sky-skybox.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::skybox::Skybox::Skybox(
    std::shared_ptr<mesh::Mesh>&& bound_mesh,
    Texture&& bound_texture,
    std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
    , bound_mesh(std::move(bound_mesh))
    , bound_texture(std::move(bound_texture))
{
}

gearoenix::render::skybox::Skybox::~Skybox() noexcept = default;

gearoenix::render::skybox::Skybox::Skybox(Skybox&&) noexcept = default;