#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/skybox/gx-rnd-sky-manager.hpp"
#include "../render/skybox/gx-rnd-sky-skybox.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Texture2D;
struct TextureCube;
struct Mesh;

struct Skybox final : render::skybox::Skybox {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_gl_skybox_type_index;
    constexpr static std::array all_parent_object_type_indices { render::skybox::Skybox::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::skybox::Skybox::object_type_index };

    GX_GET_CREF_PRV(std::shared_ptr<Mesh>, gl_mesh);
    GX_GET_VAL_PRV(uint, texture_object, static_cast<uint>(-1));

    Skybox(core::ecs::Entity* entity, std::shared_ptr<render::texture::Texture>&& texture, std::shared_ptr<Mesh>&& mesh, std::string&& name);

public:
    ~Skybox() override;
    [[nodiscard]] uint get_vertex_object() const;
    [[nodiscard]] uint get_index_buffer() const;
};

struct SkyboxManager final : render::skybox::Manager {
    SkyboxManager();
    ~SkyboxManager() override;

    [[nodiscard]] core::ecs::EntityPtr build(
        std::string&& name,
        core::ecs::Entity* parent,
        std::shared_ptr<render::texture::Texture>&& texture,
        std::shared_ptr<render::mesh::Mesh>&& mesh) override;
};
}

#endif