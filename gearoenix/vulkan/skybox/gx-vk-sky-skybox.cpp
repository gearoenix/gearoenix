#include "gx-vk-sky-skybox.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../material/gx-vk-mat-manager.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../texture/gx-vk-txt-cube.hpp"

namespace {
VkPipeline equirectangular_pipeline = nullptr;
}

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

    if (is_equirectangular && !equirectangular_pipeline) {
        equirectangular_pipeline = pipeline::Manager::get().get_skybox_equirectangular_pipeline()->get_vulkan_data();
        GX_ASSERT_D(equirectangular_pipeline);
    } else {
        GX_UNEXPECTED;
    }
}

gearoenix::vulkan::skybox::Skybox::~Skybox() = default;

void gearoenix::vulkan::skybox::Skybox::render_forward(const VkCommandBuffer cmd, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline) const
{
    if (is_equirectangular) {
        if (equirectangular_pipeline != current_bound_pipeline) {
            current_bound_pipeline = equirectangular_pipeline;
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, equirectangular_pipeline);
        }
    } else {
        GX_UNEXPECTED;
    }

    pc.material_index = material_shader_data.get_index();

    vk_mesh->draw(cmd, pc);
}
#endif
