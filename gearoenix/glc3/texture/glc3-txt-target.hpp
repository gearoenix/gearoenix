#ifndef GEAROENIX_GLC3_TEXTURE_TARGET_HPP
#define GEAROENIX_GLC3_TEXTURE_TARGET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-info.hpp"
#include <vector>

namespace gearoenix::render::texture {
class Target;
}

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
class Target {
private:
    gl::sint framebuffer = -1;
    gl::sint depth_buffer = -1;
    std::vector<gl::uint> texture_objects;
    engine::Engine* const gl_e;
    // settings
    std::optional<gl::enumerated> gl_cull_mode = GL_BACK;
    std::optional<std::pair<gl::enumerated, gl::enumerated>> gl_blend_mode = std::make_pair(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bool depth_test_enabled = true;
    bool scissor_test_enabled = true;
    bool stencil_test_enabled = true;
    bool write_depth = true;

public:
    explicit Target(engine::Engine* e) noexcept;
    ~Target() noexcept;
    [[nodiscard]] static std::shared_ptr<render::texture::Target> construct(
        core::Id id,
        engine::Engine* e,
        const std::vector<render::texture::Info>& infos,
        unsigned int width,
        unsigned int height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void state_init() const noexcept;
    void fetch_current_framebuffer() noexcept;
    void generate_framebuffer(const std::vector<render::texture::Info>& infos, unsigned int w, unsigned int h) noexcept;
    void bind() const noexcept;
    void clear() const noexcept;
    static void bind(const render::texture::Target* t) noexcept;
    void bind_textures(const gl::enumerated* texture_units, std::size_t count) const noexcept;
    static void bind_textures(const render::texture::Target* t, const gl::enumerated* texture_units, std::size_t count) noexcept;
};
}

#endif
#endif
