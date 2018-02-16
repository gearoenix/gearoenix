#ifndef GEAROENIX_OPENGL_TEXTURE_2D_HPP
#define GEAROENIX_OPENGL_TEXTURE_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/texture/rnd-txt-texture-2d.hpp"
#include "../gles3.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    class File;
}
namespace gles3 {
    class Engine;
    namespace texture {
        class Texture2D : public render::texture::Texture2D {
        private:
            GLuint texture_object;

        public:
            Texture2D(system::file::File* file, Engine* engine, core::EndCaller<core::EndCallerIgnore> end);
            Texture2D(GLuint txtobj);
            ~Texture2D();
            void bind(GLenum texture_unit);
        };
    }
}
}
#endif
#endif
