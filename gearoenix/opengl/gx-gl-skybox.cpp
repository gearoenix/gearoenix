#include "gx-gl-skybox.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-entity-builder.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-texture.hpp"
#include <boost/mp11/algorithm.hpp>

namespace {
template <typename T>
constexpr std::uint32_t index_of_texture = boost::mp11::mp_find<gearoenix::render::skybox::Texture, std::shared_ptr<T>>::value;

constexpr auto index_of_texture_2d = index_of_texture<gearoenix::render::texture::Texture2D>;
constexpr auto index_of_texture_cube = index_of_texture<gearoenix::render::texture::TextureCube>;

gearoenix::gl::Skybox::GlTexture convert(const gearoenix::render::skybox::Texture& texture)
{
    switch (texture.index()) {
    case index_of_texture_2d:
        return std::dynamic_pointer_cast<gearoenix::gl::Texture2D>(std::get<index_of_texture_2d>(texture));
    case index_of_texture_cube:
        return std::dynamic_pointer_cast<gearoenix::gl::TextureCube>(std::get<index_of_texture_cube>(texture));
    default:
        GX_UNEXPECTED;
    }
}
}

void gearoenix::gl::Skybox::write_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const
{
    GX_UNIMPLEMENTED;
}

gearoenix::gl::Skybox::Skybox(
    render::skybox::Texture&& texture,
    std::shared_ptr<Mesh>&& mesh,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : render::skybox::Skybox(
          core::ecs::ComponentType::create_index(this),
          std::shared_ptr<render::mesh::Mesh>(mesh),
          std::move(texture),
          std::move(name),
          entity_id)
    , gl_texture(::convert(bound_texture))
    , gl_mesh(std::move(mesh))
{
}

void gearoenix::gl::Skybox::construct(
    std::string&&,
    const core::ecs::entity_id_t,
    std::shared_ptr<platform::stream::Stream>&&,
    core::job::EndCallerShared<Component>&&)
{
    GX_UNIMPLEMENTED;
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

gearoenix::gl::uint gearoenix::gl::Skybox::get_texture_object() const
{
    switch (gl_texture.index()) {
    case index_of_texture_2d:
        return std::get<index_of_texture_2d>(gl_texture)->get_object();
    case index_of_texture_cube:
        return std::get<index_of_texture_cube>(gl_texture)->get_object();
    default:
        GX_UNEXPECTED;
    }
}

gearoenix::gl::SkyboxBuilder::SkyboxBuilder(
    std::string&& name,
    render::skybox::Texture&& bound_texture,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    core::job::EndCaller<>&& entity_end_callback)
    : Builder(std::move(name), std::move(entity_end_callback))
{
    entity_builder->get_builder().add_component(core::ecs::construct_component<Skybox>(
        std::move(bound_texture),
        std::static_pointer_cast<Mesh>(std::move(mesh)),
        entity_builder->get_builder().get_name() + "-gl-skybox",
        entity_builder->get_id()));
}

gearoenix::gl::SkyboxBuilder::~SkyboxBuilder() = default;

gearoenix::gl::SkyboxManager::SkyboxManager(Engine& e)
    : Manager(e)
    , eng(e)
{
    core::ecs::ComponentType::add<Skybox>();
}

gearoenix::gl::SkyboxManager::~SkyboxManager() = default;

std::shared_ptr<gearoenix::render::skybox::Builder> gearoenix::gl::SkyboxManager::build(
    std::string&& name,
    render::skybox::Texture&& bound_texture,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    core::job::EndCaller<>&& entity_end_callback)
{
    return std::make_shared<SkyboxBuilder>(std::move(name), std::move(bound_texture), std::move(mesh), std::move(entity_end_callback));
}

#endif
