#include "gx-gl-skybox.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-texture.hpp"
#include <boost/mp11/algorithm.hpp>

// void gearoenix::gl::Skybox::write_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const
// {
//     GX_UNIMPLEMENTED;
// }

gearoenix::gl::Skybox::Skybox(core::ecs::Entity* const entity, std::shared_ptr<render::texture::Texture>&& texture, std::shared_ptr<Mesh>&& mesh, std::string&& name)
    : render::skybox::Skybox(entity, core::ecs::ComponentType::create_index(this), std::shared_ptr<render::mesh::Mesh>(mesh), std::move(texture), std::move(name))
    , gl_mesh(std::move(mesh))
{
    if (const auto t2d = std::dynamic_pointer_cast<Texture2D>(bound_texture); t2d) {
        texture_object = t2d->get_object();
    } else if (const auto tc = std::dynamic_pointer_cast<TextureCube>(bound_texture); tc) {
        texture_object = tc->get_object();
    } else {
        GX_UNEXPECTED;
    }
}

gearoenix::gl::Skybox::~Skybox() = default;

gearoenix::gl::uint gearoenix::gl::Skybox::get_vertex_object() const
{
    return gl_mesh->get_cached_vertex_object(); // todo these can be cached for improving
}

gearoenix::gl::uint gearoenix::gl::Skybox::get_index_buffer() const
{
    return gl_mesh->get_cached_index_buffer();
}

gearoenix::gl::SkyboxManager::SkyboxManager()
{
    core::ecs::ComponentType::add<Skybox>();
}

gearoenix::gl::SkyboxManager::~SkyboxManager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::gl::SkyboxManager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    std::shared_ptr<render::texture::Texture>&& texture,
    std::shared_ptr<render::mesh::Mesh>&& mesh)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<Skybox>(entity.get(), std::move(texture), std::static_pointer_cast<Mesh>(std::move(mesh)), entity->get_object_name() + "-gl-skybox"));
    return entity;
}

#endif
