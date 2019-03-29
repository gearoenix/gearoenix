#ifndef GEAROENIX_GLES2_SHADER_SHADER_HPP
#define GEAROENIX_GLES2_SHADER_SHADER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../gles2.hpp"
#include <string>

#define GX_SHADER_UNIFORM_FAILED 0xFFFFFFFF

#ifdef GX_DEBUG_GLES2
#define GX_DEBUG_GLES2_GLSL
#endif

#define GX_GLES2_GET_UNIFORM(shd, uniform) uniform = shd->get_uniform_location(#uniform)

#define GX_GLES2_GET_UNIFORM_F(shd, uniform)             \
GX_GLES2_GET_UNIFORM(shd, uniform);                      \
if(GX_SHADER_UNIFORM_FAILED == uniform)                  \
{                                                        \
	GXLOGF("Failed to locate the uniform " << #uniform); \
}

#define GX_GLES2_THIS_GET_UNIFORM_F(uniform) GX_GLES2_GET_UNIFORM_F(this, uniform)

namespace gearoenix {
	namespace gles2 {
		namespace engine {
			class Engine;
		}
		namespace shader {
			class Shader {
			protected:
				const std::shared_ptr<engine::Engine> e;
				GLuint shader_program = 0;
				GLuint vertex_object = 0;
				GLuint fragment_object = 0;
				GLint position_attribute_location = -1;
				GLint normal_attribute_location = -1;
				GLint tangent_attribute_location = -1;
				GLint uv_attribute_location = -1;
				void create_program();
				void run();
				void link();
				void validate();
				GLuint add_shader_to_program(const std::string& shd, const GLenum shader_type);
				GLuint set_vertex_shader(const std::string& shd);
				GLuint set_fragment_shader(const std::string& shd);
				static void end_program(const GLuint shader_program);
				static void end_object(const GLuint shader_object);
			public:
				Shader(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
				virtual ~Shader();
				/// On not found returns GX_SHADER_UNIFORM_FAILED
				GLuint get_uniform_location(const std::string& name) const;
			};
		}
	}
}
#endif
#endif
