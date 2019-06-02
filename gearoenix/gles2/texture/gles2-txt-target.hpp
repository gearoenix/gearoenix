#ifndef GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#define GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-target.hpp"
namespace gearoenix::gles2 {
namespace engine {
    class Engine;
}
namespace texture {
    class Target : public render::texture::Target {
    private:
        gl::uint texture_object = 0;
        gl::sint framebuffer = 0;
        gl::sint depth_buffer = 0;

    public:
        explicit Target(engine::Engine* e) noexcept;
        ~Target() noexcept final;
        void bind() const noexcept;
    };
}
}
#endif
#endif
