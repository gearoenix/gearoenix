#ifndef GEAROENIX_GLC3_ENGINE_HPP
#define GEAROENIX_GLC3_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gl-types.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../glc3.hpp"

namespace gearoenix::glc3::texture {
class Texture2D;
class Target;
}

namespace gearoenix::glc3::engine {
class Engine final : public render::engine::Engine {
private:
    const texture::Target* bound_target = nullptr;
    gl::uint bound_shader = gl::uint(-1);

    explicit Engine(system::Application* sys_app, render::engine::Type engine_type) noexcept;

public:
    static Engine* construct(system::Application* sys_app, render::engine::Type engine_type) noexcept;
    ~Engine() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
    [[nodiscard]] std::shared_ptr<render::sync::Semaphore> create_semaphore() const noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_texture_2d(
        core::Id id,
        std::string name,
        std::vector<std::vector<std::uint8_t>> data,
        const render::texture::TextureInfo& info,
        std::size_t img_width,
        std::size_t img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::TextureCube> create_texture_cube(
        core::Id id,
        std::string name,
        std::vector<std::vector<std::vector<std::uint8_t>>> data,
        const render::texture::TextureInfo& info,
        std::size_t aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Target> create_render_target(
        core::Id id,
        const std::vector<render::texture::AttachmentInfo>& infos,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    void submit(
        std::size_t pres_count,
        const render::sync::Semaphore* const* pres,
        std::size_t cmds_count,
        const render::command::Buffer* const* cmds,
        std::size_t nxts_count,
        const render::sync::Semaphore* const* nxts) noexcept final;
};
}
#endif
#endif
