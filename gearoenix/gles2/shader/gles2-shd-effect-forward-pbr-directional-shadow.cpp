#include "gles2-shd-effect-forward-pbr-directional-shadow.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/sys-log.hpp"

const static std::string vertex_shader_code =
	// precision(s)
	"precision highp float;\n"
	"precision highp sampler2D;\n"
	"precision highp samplerCube;\n"
	// attribute(s)
	"attribute vec3 position;\n"
	"attribute vec3 normal;\n"
	"attribute vec4 tangent;\n"
	"attribute vec2 uv;\n"
	// effect uniform(s)
	"uniform mat4 effect_mvp;\n"
	// light uniform(s)
	"uniform mat4 light_vp_bias;\n"
	// model uniform(s)
	"uniform mat4 model_m;\n"
	// output(s)
	"varying vec3 out_pos;\n"
	"varying mat3 out_nrm;\n"
	"varying mat3 out_tng;\n"
	"varying mat3 out_tbn;\n"
	"varying vec2 out_uv;\n"
	"varying vec3 out_light_pos;\n"
	"void main()\n"
	"{\n"
	"    const vec4 pos = m * vec4(position, 1.0);\n"
	"    out_pos = pos.xyz;\n"
	"    out_nrm = normalize((m * vec4(normal, 0.0)).xyz);\n"
	"    out_tng = normalize((m * vec4(tangent.xyz, 0.0)).xyz);\n"
	"    out_btg = cross(nrm, tng) * tangent.w;\n"
	"    out_uv = uv;\n"
	"    const vec4 light_pos = light_vp_bias * pos;\n"
	"    out_light_pos = light_pos.xyz / light_pos.w;\n"
	"    gl_Position = mvp * pos;\n"
	"}";

const static std::string fragment_shader_code = 
// precisions
"precision highp float;\n"
"precision highp sampler2D;\n"
"precision highp samplerCube;\n"
// constant(s)
"const float PI = 3.14159265359;\n"
// material uniform(s)
"uniform float     material_alpha;\n"
"uniform float     material_alpha_cutoff;\n"
"uniform float     material_metallic_factor;\n"
"uniform float     material_normal_scale;\n"
"uniform float     material_occlusion_strength;\n"
"uniform float     material_roughness_factor;\n"
"uniform sampler2D material_base_color;\n"
"uniform sampler2D material_metallic_roughness;\n"
"uniform sampler2D material_normal;\n"
"uniform sampler2D material_emissive;\n"
// scenes uniform(s)
"uniform vec3 scene_ambient_light;\n"
"uniform vec3 scene_directional_lights_color[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n"
"uniform vec3 scene_directional_lights_direction[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n"
"uniform vec4 scene_point_lights_color_min_radius[" GX_MAX_POINT_LIGHTS_STR "];\n"
"uniform vec4 scene_point_lights_position_max_radius[" GX_MAX_POINT_LIGHTS_STR "];\n"
// directional, point
"uniform vec2 scene_lights_count;\n"
// samples-count, radius, z-tolerance
"uniform vec3 scene_ssao_config;\n"
// effect uniforms
"uniform sampler2D effect_diffuse_environment;\n"
"uniform sampler2D effect_specular_environment;\n"
"uniform sampler2D effect_ambient_occlusion;\n"
"uniform sampler2D effect_shadow_map;\n"
"uniform sampler2D effect_brdflut;\n"
// light uniform(s)
"uniform vec3 light_color;\n"
"uniform vec3 light_direction;\n"
// camera uniform(s)
"uniform vec3 camera_position;\n"
;

gearoenix::gles2::shader::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: Shader(e, c)
{
}

gearoenix::gles2::shader::ForwardPbrDirectionalShadow::~ForwardPbrDirectionalShadow()
{
}

#endif
