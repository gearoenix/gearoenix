#ifndef GEAROENIX_GLES2_TEXTURE_HPP
#define GEAROENIX_GLES2_TEXTURE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-types.hpp"

namespace gearoenix::render::texture {
class Texture;
}

namespace gearoenix::gles2::texture {
class Texture {
public:
    Texture() = delete;
    static void bind(const render::texture::Texture* t, gl::enumerated texture_unit) noexcept;
};
}
#endif
#endif
