#pragma once
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

    constexpr static core::ecs::component_index_t TYPE_INDEX = 27;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES { render::skybox::Skybox::TYPE_INDEX };
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES { render::skybox::Skybox::TYPE_INDEX };

    GX_GET_CREF_PRV(GlTexture, gl_texture);
    GX_GET_CREF_PRV(std::shared_ptr<Mesh>, gl_mesh);

    void write_in_io_context(std::shared_ptr<platform::stream::Stream>&& stream,
        core::job::EndCaller<>&& end_callback) const override;

public:
    Skybox(
        render::skybox::Texture&& texture,
        std::shared_ptr<Mesh>&& mesh,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    static void construct(
        std::string&& name,
        core::ecs::entity_id_t entity_id,
        std::shared_ptr<platform::stream::Stream>&& stream,
        core::job::EndCallerShared<Component>&& end);
    ~Skybox() override;
    [[nodiscard]] uint get_vertex_object() const;
    [[nodiscard]] uint get_index_buffer() const;
    [[nodiscard]] uint get_texture_object() const;
};

struct SkyboxBuilder final : render::skybox::Builder {
    SkyboxBuilder(
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