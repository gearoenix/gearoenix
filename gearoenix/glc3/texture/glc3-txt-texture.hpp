#ifndef GEAROENIX_GLC3_TEXTURE_HPP
#define GEAROENIX_GLC3_TEXTURE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-format.hpp"
#include "../../render/texture/rnd-txt-sample.hpp"
#include "../../render/texture/rnd-txt-texture-2d.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
class Texture {
public:
    Texture() = delete;
    static void bind(const render::texture::Texture* const t, gl::enumerated texture_unit) noexcept;
};
}

#endif
#endif
