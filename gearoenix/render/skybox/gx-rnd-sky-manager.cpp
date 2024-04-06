#include "gx-rnd-sky-manager.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::skybox::Manager::Manager(engine::Engine& e)
    : e(e)
{
}

gearoenix::render::skybox::Manager::~Manager() = default;

void gearoenix::render::skybox::Manager::build(
    const std::string& name,
    const platform::stream::Path& texture_path,
    core::job::EndCaller<>&& entity_end_callback,
    core::job::EndCallerShared<Builder>&& builder_callback)
{
    if (texture_path.get_raw_data().ends_with(".hdr") || texture_path.get_raw_data().ends_with(".png")) {
        e.get_texture_manager()->create_2d_from_file(
            name + "-texture",
            texture_path,
            texture::TextureInfo(
                    texture::TextureFormat::Unknown,
                texture::SamplerInfo(
                    texture::Filter::Linear,
                    texture::Filter::Linear,
                    texture::Wrap::ClampToEdge,
                    texture::Wrap::ClampToEdge,
                    texture::Wrap::ClampToEdge),
                    0, 0, 0, texture::Type::Unknown, false),
            core::job::EndCallerShared<texture::Texture2D>([this, end = std::move(entity_end_callback), n = name, b = std::move(builder_callback)](std::shared_ptr<texture::Texture2D>&& t) mutable {
                build(std::move(n), std::move(t), std::move(end), std::move(b));
            }));
    } else if (texture_path.get_raw_data().ends_with(".gx-cube-texture")) {
        e.get_texture_manager()->read_gx3d(
            texture_path,
            core::job::EndCallerShared<texture::Texture>([this, end = std::move(entity_end_callback), n = name, b = std::move(builder_callback)](std::shared_ptr<texture::Texture>&& t) mutable {
                auto cube = std::dynamic_pointer_cast<texture::TextureCube>(std::move(t));
                GX_ASSERT(nullptr != cube);
                build(std::move(n), std::move(cube), std::move(end), std::move(b));
            }));
    }
    GX_UNEXPECTED;
}

void gearoenix::render::skybox::Manager::build(
    std::string&& name,
    Texture&& bound_texture,
    core::job::EndCaller<>&& entity_end_callback,
    core::job::EndCallerShared<Builder>&& builder_callback)
{
    e.get_mesh_manager()->build_inward_cube(
        core::job::EndCallerShared<mesh::Mesh>([this, t = std::move(bound_texture), end = std::move(entity_end_callback), n = std::move(name), b = std::move(builder_callback)](std::shared_ptr<mesh::Mesh>&& m) mutable {
            b.set_return(build(std::move(n), std::move(t), std::move(m), std::move(end)));
        }));
}
