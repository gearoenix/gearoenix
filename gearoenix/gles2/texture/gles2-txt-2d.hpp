#ifndef GEAROENIX_OPENGL_TEXTURE_2D_HPP
#define GEAROENIX_OPENGL_TEXTURE_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/texture/rnd-txt-texture-2d.hpp"
#include "../gles2.hpp"
#include <memory>
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace texture {
        class Texture2D : public render::texture::Texture2D {
        private:
            GLuint texture_object;

        public:
            Texture2D(core::Id my_id, system::stream::Stream* file, Engine* engine, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            Texture2D(core::Id my_id, GLuint txtobj, Engine* eng);
            ~Texture2D();
            void bind(GLenum texture_unit);
        };
    }
}
}
#endif
#endif
