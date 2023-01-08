#include "gx-rnd-sky-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-sky-skybox.hpp"

gearoenix::render::skybox::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    std::variant<std::shared_ptr<texture::Texture2D>, std::shared_ptr<texture::TextureCube>>&& bound_texture,
    const core::sync::EndCaller& end_callback) noexcept
    : entity_builder(e.get_world()->create_shared_builder(std::string(name), core::sync::EndCaller(end_callback)))
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(Skybox(
        e.get_mesh_manager()->build_inward_cube(core::sync::EndCaller(end_callback)),
        std::move(bound_texture),
        name + "-skybox"));
}

gearoenix::render::skybox::Builder::~Builder() noexcept = default;