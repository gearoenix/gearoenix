#ifndef GEAROENIX_GLES2_ENGINE_HPP
#define GEAROENIX_GLES2_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-types.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../gles2.hpp"
#ifdef GX_GLES2_PROFILING
#define GX_GLES2_ENGINE_PROFILING
#endif
#ifdef GX_GLES2_ENGINE_PROFILING
#include <chrono>
#endif

namespace gearoenix::gles2::texture {
class Target;
}

namespace gearoenix::gles2::engine {
class Engine final : public render::engine::Engine {
private:
    const texture::Target* bound_target = nullptr;
    gl::uint bound_shader = gl::uint(-1);

    void initialize() noexcept;
#ifdef GX_GLES2_ENGINE_PROFILING
    /// todo: create a class for profiling
    unsigned int prof_frames_count = 0;
    std::chrono::high_resolution_clock::time_point prof_last_time_draw;
    double prof_frames_time = 0.0;
#endif
    explicit Engine(system::Application* sys_app) noexcept;

public:
    static Engine* construct(system::Application* sys_app) noexcept;
    ~Engine() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
    [[nodiscard]] std::shared_ptr<render::sync::Semaphore> create_semaphore() const noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_texture_2d(
        core::Id id,
        const void* data,
        const render::texture::TextureInfo& info,
        unsigned int img_width,
        unsigned int img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::TextureCube> create_texture_cube(
        core::Id id,
        const void* data,
        const render::texture::TextureInfo& info,
        unsigned int aspect,
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
