#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::texture {
struct Texture;
}

namespace gearoenix::render::skybox {
struct Skybox : core::ecs::Component {
    constexpr static auto object_type_index = gearoenix_render_skybox_type_index;
    constexpr static std::uint32_t max_count = 16;

    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, bound_mesh);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture>, bound_texture);
    GX_GETSET_VAL_PRT(double, layer, 0.0);

    Skybox(core::ecs::Entity* entity, core::object_type_index_t final_component_type_index, std::shared_ptr<mesh::Mesh>&& bound_mesh, std::shared_ptr<texture::Texture>&& bound_texture, std::string&& name);

public:
    ~Skybox() override;
    [[nodiscard]] bool is_equirectangular() const;
    [[nodiscard]] bool is_cube() const;
};
}
