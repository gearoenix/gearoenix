#include "gx-vk-sky-skybox.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
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

void gearoenix::vulkan::skybox::Skybox::render_forward(const vk::CommandBuffer cmd, const pipeline::FormatPipelines& fp, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline) const
{
    const auto vk_pipeline = (is_equirectangular ? fp.skybox_equirectangular : fp.skybox_cube)->get_vulkan_data();
    if (vk_pipeline != current_bound_pipeline) {
        current_bound_pipeline = vk_pipeline;
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, vk_pipeline);
    }
    pc.material_index = material_shader_data.get_index();
    vk_mesh->draw(cmd, pc);
}
#endif
