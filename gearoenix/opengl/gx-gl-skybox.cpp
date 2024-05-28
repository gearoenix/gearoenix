#include "gx-gl-skybox.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-texture.hpp"
#include <boost/mp11/algorithm.hpp>

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::gl::Skybox, gearoenix::gl::Skybox::MAX_COUNT>::construct();

template <typename T>
constexpr size_t index_of_texture = boost::mp11::mp_find<gearoenix::render::skybox::Texture, std::shared_ptr<T>>::value;

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

const boost::container::flat_set<std::type_index>& gearoenix::gl::Skybox::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types { create_type_index<render::skybox::Skybox>(), create_this_type_index(this) };
    return types;
}

gearoenix::gl::Skybox::Skybox(
    render::skybox::Texture&& texture,
    std::shared_ptr<Mesh>&& mesh,
    std::string&& name)
    : render::skybox::Skybox(
          create_this_type_index(this),
          std::shared_ptr<render::mesh::Mesh>(mesh),
          std::move(texture),
          std::move(name))
    , gl_texture(::convert(bound_texture))
    , gl_mesh(std::move(mesh))
{
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

std::shared_ptr<gearoenix::gl::Skybox> gearoenix::gl::Skybox::construct(
    render::skybox::Texture&& texture,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    std::string&& name)
{
    auto self = allocator->make_shared(std::move(texture), std::dynamic_pointer_cast<Mesh>(std::move(mesh)), std::move(name));
    self->set_component_self(self);
    return self;
}

gearoenix::gl::SkyboxBuilder::SkyboxBuilder(
    Engine& e,
    std::string&& name,
    render::skybox::Texture&& bound_texture,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    core::job::EndCaller<>&& entity_end_callback)
    : Builder(e, std::move(name), std::move(entity_end_callback))
{
    entity_builder->get_builder().add_component(Skybox::construct(
        std::move(bound_texture),
        std::move(mesh),
        entity_builder->get_builder().get_name() + "-gl-skybox"));
}

gearoenix::gl::SkyboxBuilder::~SkyboxBuilder() = default;

gearoenix::gl::SkyboxManager::SkyboxManager(Engine& e)
    : Manager(e)
    , eng(e)
{
}

gearoenix::gl::SkyboxManager::~SkyboxManager() = default;

std::shared_ptr<gearoenix::render::skybox::Builder> gearoenix::gl::SkyboxManager::build(
    std::string&& name,
    render::skybox::Texture&& bound_texture,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    core::job::EndCaller<>&& entity_end_callback)
{
    return std::make_shared<SkyboxBuilder>(eng, std::move(name), std::move(bound_texture), std::move(mesh), std::move(entity_end_callback));
}

#endif
