#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/skybox/gx-rnd-sky-skybox.hpp"
#include "../material/gx-vk-mat-material.hpp"
#include "../mesh/gx-vk-msh-draw-cache.hpp"
#include "../mesh/gx-vk-msh-mesh.hpp"

namespace gearoenix::vulkan {
struct DrawState;
}

namespace gearoenix::vulkan::skybox {
struct Skybox final : render::skybox::Skybox {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_gapi_skybox_type_index;
    constexpr static std::array all_parent_object_type_indices { render::skybox::Skybox::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::skybox::Skybox::object_type_index };

private:
    std::shared_ptr<mesh::Mesh> vk_mesh;
    mesh::DrawCache draw_cache;

    Skybox(
        core::ecs::Entity* entity,
        std::shared_ptr<render::mesh::Mesh>&& in_mesh,
        std::shared_ptr<render::texture::Texture>&& in_txt,
        std::string&& name);

public:
    ~Skybox() override;
    void render_forward(DrawState& draw_state) const;
};
}
#endif
