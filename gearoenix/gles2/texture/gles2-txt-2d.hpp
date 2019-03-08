#ifndef GEAROENIX_OPENGL_TEXTURE_2D_HPP
#define GEAROENIX_OPENGL_TEXTURE_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/texture/rnd-txt-texture-2d.hpp"
#include "../gles2.hpp"
#include <memory>
namespace gearoenix {
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace texture {
        class Texture2D : public render::texture::Texture2D {
        private:
            GLuint texture_object;

        public:
            Texture2D(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& file,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            Texture2D(const core::Id my_id, const GLuint txtobj, const std::shared_ptr<engine::Engine>& e);
            ~Texture2D();
            void bind(const GLenum texture_unit);
        };
    }
}
}
#endif
#endif
