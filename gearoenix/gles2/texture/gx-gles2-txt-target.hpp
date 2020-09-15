#ifndef GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#define GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../gl/gx-gl-constants.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../render/texture/gx-rnd-txt-target.hpp"
#include <optional>
#include <vector>

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::texture {
class Texture2D;

struct Framebuffer {
    gl::sint framebuffer = -1;
    gl::sint depth_buffer = -1;
    bool borrowed = false;
    engine::Engine* gl_e = nullptr;

    ~Framebuffer() noexcept;
    void bind() const noexcept;
};

class Target final : public render::texture::Target {
    GX_GET_CREF_PRV(std::shared_ptr<Framebuffer>, framebuffer)
    GX_GET_CPTR_PRV(engine::Engine, gl_e)
private:
    std::optional<gl::enumerated> gl_cull_mode = GL_BACK;
    std::optional<std::pair<gl::enumerated, gl::enumerated>> gl_blend_mode = std::make_pair(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Target(core::Id id, engine::Engine* e) noexcept;
    Target(const Target&) noexcept;

    void initialize_texture(
        const render::texture::AttachmentInfo& info,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void state_init() const noexcept;
    void fetch_current_framebuffer() noexcept;
    void generate_framebuffer() noexcept;

public:
    explicit Target(engine::Engine* e) noexcept;
    ~Target() noexcept final;
    [[nodiscard]] static std::shared_ptr<render::texture::Target> construct(
        core::Id id,
        engine::Engine* e,
        const render::texture::AttachmentInfo& info,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void bind(const Target*) const noexcept;
    void clear(const Target*) const noexcept;
    //    static void bind(const render::texture::Target* target) noexcept;
    //    static void bind_texture(const render::texture::Target* t, gl::enumerated texture_unit) noexcept;
    [[nodiscard]] render::texture::Target* clone() const noexcept final;
};
}

#endif
#endif
