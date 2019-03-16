#ifndef GEAROEMIX_GLES2_TEXTURE_SAMPLE_HPP
#define GEAROEMIX_GLES2_TEXTURE_SAMPLE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../gles2.hpp"
#include "../../render/texture/rnd-txt-sample.hpp"

namespace gearoenix {
namespace gles2 {
    namespace texture {
        struct SampleInfo {
			GLfloat min_filter = GL_LINEAR_MIPMAP_LINEAR;
			GLfloat mag_filter = GL_LINEAR;
			GLint wrap_s = GL_REPEAT;
			GLint wrap_t = GL_REPEAT;
			GLint wrap_r = GL_REPEAT;

			SampleInfo();
			SampleInfo(const render::texture::SampleInfo& o);
        };
    }
}
}
#endif
#endif
