#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/skybox/gx-rnd-sky-skybox.hpp"
#include "../material/gx-vk-mat-material.hpp"
#include "../mesh/gx-vk-msh-mesh.hpp"

namespace gearoenix::vulkan::skybox {
struct Skybox final : render::skybox::Skybox {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_gapi_skybox_type_index;
    constexpr static std::array all_parent_object_type_indices { render::skybox::Skybox::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::skybox::Skybox::object_type_index };

    GX_GET_CREF_PRV(material::uniform_indexer_t::DataAccess, material_shader_data);
    GX_GET_REFC_PRV(std::shared_ptr<mesh::Mesh>, vk_mesh);

    Skybox(
        core::ecs::Entity* entity,
        std::shared_ptr<render::mesh::Mesh>&& in_mesh,
        std::shared_ptr<render::texture::Texture>&& in_txt,
        std::string&& name);

public:
    ~Skybox() override;
    void render_forward(VkCommandBuffer cmd, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline) const;
};
}
#endif
