#include "gx-gl-skybox.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../platform/gx-plt-log.hpp"
#include "../render/skybox/gx-rnd-sky-skybox.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::Skybox::Skybox(std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>>&& texture, std::shared_ptr<Mesh>&& mesh) noexcept
    : core::ecs::Component(this)
    , texture(std::move(texture))
    , mesh(std::move(mesh))
{
}

gearoenix::gl::Skybox::~Skybox() noexcept = default;

gearoenix::gl::Skybox::Skybox(Skybox&&) noexcept = default;

gearoenix::gl::uint gearoenix::gl::Skybox::get_vertex_object() const noexcept
{
    return mesh->vertex_object;
}

gearoenix::gl::uint gearoenix::gl::Skybox::get_index_buffer() const noexcept
{
    return mesh->index_buffer;
}

gearoenix::gl::uint gearoenix::gl::Skybox::get_texture_object() const noexcept
{
    return texture.index() == 0 ? std::get<0>(texture)->get_object() : std::get<1>(texture)->get_object();
}

gearoenix::gl::SkyboxBuilder::SkyboxBuilder(
    Engine& e,
    const std::string& name,
    std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>>&& bound_texture,
    const core::sync::EndCallerIgnored& end_callback) noexcept
    : render::skybox::Builder(
        e,
        name,
        bound_texture.index() == 0 ? std::variant<std::shared_ptr<render::texture::Texture2D>, std::shared_ptr<render::texture::TextureCube>>(std::get<0>(bound_texture)) : std::variant<std::shared_ptr<render::texture::Texture2D>, std::shared_ptr<render::texture::TextureCube>>(std::get<1>(bound_texture)),
        end_callback)
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(Skybox(std::move(bound_texture),
        std::dynamic_pointer_cast<Mesh>(builder.get_component<render::skybox::Skybox>()->get_bound_mesh())));
}

gearoenix::gl::SkyboxBuilder::~SkyboxBuilder() noexcept = default;

gearoenix::gl::SkyboxManager::SkyboxManager(Engine& e) noexcept
    : render::skybox::Manager(e)
    , eng(e)
{
}

gearoenix::gl::SkyboxManager::~SkyboxManager() noexcept = default;

std::shared_ptr<gearoenix::render::skybox::Builder> gearoenix::gl::SkyboxManager::build(
    std::string&& name, Texture&& bound_texture, const core::sync::EndCallerIgnored& c) noexcept
{
    std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>> texture;
    if (bound_texture.index() == 0)
        texture = std::dynamic_pointer_cast<Texture2D>(std::get<0>(bound_texture));
    else if (bound_texture.index() == 1)
        texture = std::dynamic_pointer_cast<TextureCube>(std::get<1>(bound_texture));
    else
        GX_UNEXPECTED;
    return std::make_shared<SkyboxBuilder>(eng, std::move(name), std::move(texture), c);
}

#endif
