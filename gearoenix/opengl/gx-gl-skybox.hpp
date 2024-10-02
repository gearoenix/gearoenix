#ifndef GEAROENIX_GL_SKYBOX_HPP
#define GEAROENIX_GL_SKYBOX_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/skybox/gx-rnd-sky-builder.hpp"
#include "../render/skybox/gx-rnd-sky-manager.hpp"
#include "../render/skybox/gx-rnd-sky-skybox.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Engine;
struct Texture2D;
struct TextureCube;
struct Mesh;

struct Skybox final : render::skybox::Skybox {
    typedef std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>> GlTexture;

    GX_GET_CREF_PRV(GlTexture, gl_texture);
    GX_GET_CREF_PRV(std::shared_ptr<Mesh>, gl_mesh);

    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;

public:
    Skybox(
        render::skybox::Texture&& texture,
        std::shared_ptr<Mesh>&& mesh,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    [[nodiscard]] static std::shared_ptr<Skybox> construct(
        render::skybox::Texture&& texture,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    ~Skybox() override;
    [[nodiscard]] uint get_vertex_object() const;
    [[nodiscard]] uint get_index_buffer() const;
    [[nodiscard]] uint get_texture_object() const;
};

struct SkyboxBuilder final : render::skybox::Builder {
    SkyboxBuilder(
        Engine& e,
        std::string&& name,
        render::skybox::Texture&& bound_texture,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        core::job::EndCaller<>&& entity_end_callback);
    ~SkyboxBuilder() override;
};

struct SkyboxManager final : render::skybox::Manager {
    Engine& eng;

    explicit SkyboxManager(Engine& e);
    ~SkyboxManager() override;
    [[nodiscard]] std::shared_ptr<render::skybox::Builder> build(
        std::string&& name,
        render::skybox::Texture&& bound_texture,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        core::job::EndCaller<>&& entity_end_callback) override;
};
}

#endif
#endif