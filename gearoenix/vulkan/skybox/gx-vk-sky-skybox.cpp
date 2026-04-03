#include "gx-vk-sky-skybox.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../gx-vk-draw-state.hpp"
#include "../material/gx-vk-mat-manager.hpp"
#include "../pipeline/gx-vk-pip-format-pipelines.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../texture/gx-vk-txt-cube.hpp"

gearoenix::vulkan::skybox::Skybox::Skybox(
    core::ecs::Entity* const entity,
    std::shared_ptr<render::mesh::Mesh>&& in_mesh,
    std::shared_ptr<render::texture::Texture>&& in_txt,
    std::string&& name)
    : render::skybox::Skybox(entity, core::ecs::ComponentType::create_index(this), std::move(in_mesh), std::move(in_txt), std::move(name))
    , material_shader_data(material::uniform_indexer_t::get().get_next())
    , vk_mesh(core::cast_shared<mesh::Mesh>(std::shared_ptr(bound_mesh)))
{
    vk_mesh->set(false, draw_cache);
    draw_cache.material_draw_cache.material_index = material_shader_data.get_index();
    draw_cache.material_draw_cache.forward_pipeline_index =
        static_cast<std::uint8_t>(is_equirectangular ? pipeline::FormatPipelinesIndices::skybox_equirectangular_index : pipeline::FormatPipelinesIndices::skybox_cube_index);

    auto& r = *material_shader_data.get_ptr();

    r.albedo_factor = math::Vec4(1.0f);

    if (const auto t2d = std::dynamic_pointer_cast<texture::Texture2D>(bound_texture); t2d) {
        r.albedo_texture_index = t2d->get_view_index();
        r.albedo_sampler_index = t2d->get_sampler_index();
    } else if (const auto tc = std::dynamic_pointer_cast<texture::TextureCube>(bound_texture); tc) {
        r.albedo_texture_index = tc->get_view_index();
        r.albedo_sampler_index = tc->get_sampler_index();
    } else {
        GX_UNEXPECTED;
    }
}

gearoenix::vulkan::skybox::Skybox::~Skybox() = default;

void gearoenix::vulkan::skybox::Skybox::render_forward(DrawState& draw_state) const
{
    draw_mesh(draw_state, draw_cache, draw_cache.material_draw_cache.forward_pipeline_index);
}
#endif
