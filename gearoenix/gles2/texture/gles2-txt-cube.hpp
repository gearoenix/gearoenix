#ifndef GEAROENIX_OPENGL_TEXTURE_CUBE_HPP
#define GEAROENIX_OPENGL_TEXTURE_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/texture/rnd-txt-texture-cube.hpp"
#include "../gles2.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    class File;
}
namespace gles2 {
    class Engine;
    namespace texture {
        class Cube : public render::texture::Cube {
        private:
            GLuint texture_object;

        public:
            Cube(core::Id my_id, system::stream::Stream* file, Engine* engine, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            ~Cube();
            void bind(GLenum texture_unit);
        };
    }
}
}
#endif
#endif
