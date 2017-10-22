#ifndef GEAROENIX_GLES2_SHADER_COMPILER_HPP
#define GEAROENIX_GLES2_SHADER_COMPILER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/shader/rnd-shd-shader.hpp"
#include <SDL2/SDL_opengles2.h>
#include <string>
namespace gearoenix {
namespace gles2 {
    namespace shader {
        class Shader : public render::shader::Shader {
        protected:
            GLuint shader_program;
            void create_program();
            void run();
            void link();
            void validate();
            GLuint add_shader_to_program(const std::string& shd, const GLenum& shader_type);
            GLuint get_uniform_location(const std::string& name);
            void end_program();
            void end_object(const GLuint& shader_object);

        public:
            Shader();
            virtual ~Shader();
        };
    }
}
}
#endif
#endif
