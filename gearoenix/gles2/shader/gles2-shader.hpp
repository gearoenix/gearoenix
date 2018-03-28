#ifndef GEAROENIX_GLES2_SHADER_COMPILER_HPP
#define GEAROENIX_GLES2_SHADER_COMPILER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../core/cr-end-caller.hpp"
#include "../../render/shader/rnd-shd-shader.hpp"
#include "../gles2.hpp"
#include <string>
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace shader {
        class Shader : public render::shader::Shader {
        protected:
            Engine* eng;
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
            Shader(core::Id my_id, Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            virtual ~Shader();
        };
    }
}
}
#endif
#endif
