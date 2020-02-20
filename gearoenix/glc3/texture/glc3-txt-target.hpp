#ifndef GEAROENIX_GLC3_TEXTURE_TARGET_HPP
#define GEAROENIX_GLC3_TEXTURE_TARGET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-target.hpp"
#include <optional>
#include <vector>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
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

    void initialize_textures(
        const std::vector<render::texture::AttachmentInfo>& infos,
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
        const std::vector<render::texture::AttachmentInfo>& infos,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void bind() const noexcept;
    void clear() const noexcept;
    //    void bind_textures(const gl::enumerated* texture_units, std::size_t count) const noexcept;
    [[nodiscard]] render::texture::Target* clone() const noexcept final;
};
}

#endif
#endif
