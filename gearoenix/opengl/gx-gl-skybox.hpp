#ifndef GEAROENIX_GL_SKYBOX_HPP
#define GEAROENIX_GL_SKYBOX_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/skybox/gx-rnd-sky-builder.hpp"
#include "../render/skybox/gx-rnd-sky-manager.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Engine;
struct Texture2D;
struct TextureCube;
struct Mesh;

struct Skybox final : core::ecs::Component {
    std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>> texture;
    std::shared_ptr<Mesh> mesh;

    Skybox(
        std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>>&& texture,
        std::shared_ptr<Mesh>&& mesh) noexcept;
    ~Skybox() noexcept final;
    Skybox(Skybox&&) noexcept;
    [[nodiscard]] uint get_vertex_object() const noexcept;
    [[nodiscard]] uint get_index_buffer() const noexcept;
    [[nodiscard]] uint get_texture_object() const noexcept;
};

struct SkyboxBuilder final : public render::skybox::Builder {
    SkyboxBuilder(
        Engine& e,
        const std::string& name,
        std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>>&& bound_texture,
        const core::sync::EndCaller& end_callback) noexcept;
    ~SkyboxBuilder() noexcept final;
};

struct SkyboxManager final : public render::skybox::Manager {
    Engine& eng;

    explicit SkyboxManager(Engine& e) noexcept;
    ~SkyboxManager() noexcept final;
    [[nodiscard]] std::shared_ptr<render::skybox::Builder> build(
        std::string&& name,
        Texture&& bound_texture,
        const core::sync::EndCaller& c) noexcept final;
};
}

#endif
#endif