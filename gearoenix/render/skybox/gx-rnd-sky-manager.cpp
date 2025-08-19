#include "gx-rnd-sky-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-sky-skybox.hpp"

gearoenix::render::skybox::Manager::Manager()
    : Singleton(this)
{
    core::ecs::ComponentType::add<Skybox>();
}

gearoenix::render::skybox::Manager::~Manager() = default;

void gearoenix::render::skybox::Manager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    const platform::stream::Path& texture_path,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    if (texture_path.get_raw_data().ends_with(".hdr") || texture_path.get_raw_data().ends_with(".png")) {
        texture::Manager::get().create_2d_from_file(
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
            core::job::EndCallerShared<texture::Texture2D>([this, end = std::move(entity_callback), n = name, parent](std::shared_ptr<texture::Texture2D>&& t) mutable -> void {
                build(std::move(n), parent, std::move(t), std::move(end));
            }));
        return;
    }
    if (texture_path.get_raw_data().ends_with(".gx-cube-texture")) {
        texture::Manager::get().read_gx3d(
            texture_path,
            core::job::EndCallerShared<texture::Texture>([this, end = std::move(entity_callback), n = name, parent](std::shared_ptr<texture::Texture>&& t) mutable -> void {
                auto cube = std::dynamic_pointer_cast<texture::TextureCube>(std::move(t));
                GX_ASSERT(nullptr != cube);
                build(std::move(n), parent, std::move(cube), std::move(end));
            }));
        return;
    }
    GX_UNEXPECTED;
}

void gearoenix::render::skybox::Manager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    Texture&& bound_texture,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    material::Manager::get().get_unlit(
        "dummy",
        core::job::EndCallerShared<material::Unlit>([this, t = std::move(bound_texture), end = std::move(entity_callback), n = std::move(name), parent](std::shared_ptr<material::Unlit>&& mat) mutable -> void {
            mesh::Manager::get().build_inward_cube(
                std::move(mat),
                core::job::EndCallerShared<mesh::Mesh>([this, t = std::move(t), end = std::move(end), n = std::move(n), parent](std::shared_ptr<mesh::Mesh>&& m) mutable -> void {
                    end.set_return(build(std::move(n), parent, std::move(t), std::move(m)));
                }));
        }));
}
