#ifndef GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#define GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-types.hpp"
#include <optional>
#include <vector>

namespace gearoenix::render::texture {
class Target;
struct TextureInfo;
}

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::texture {
class Texture2D;
class Target {
private:
    engine::Engine* const gl_e;
    gl::sint framebuffer = -1;
    gl::sint depth_buffer = -1;
    // settings
    std::optional<gl::enumerated> gl_cull_mode = GL_BACK;
    std::optional<std::pair<gl::enumerated, gl::enumerated>> gl_blend_mode = std::make_pair(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bool depth_test_enabled = true;
    bool scissor_test_enabled = true;
    bool stencil_test_enabled = true;
    bool write_depth = true;
    bool framebuffer_borrowed = false;

public:
    explicit Target(engine::Engine* e) noexcept;
    ~Target() noexcept;
    [[nodiscard]] static std::shared_ptr<render::texture::Target> construct(
        core::Id id,
        engine::Engine* e,
        const render::texture::TextureInfo& infos,
        unsigned int width,
        unsigned int height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void state_init() const noexcept;
    void fetch_current_framebuffer() noexcept;
    void generate_framebuffer(const Texture2D* txt, unsigned int w, unsigned int h) noexcept;
    void bind() const noexcept;
    void clear() const noexcept;
    static void bind(const render::texture::Target* target) noexcept;
    static void bind_texture(const render::texture::Target* t, gl::enumerated texture_unit) noexcept;
};
}

#endif
#endif
