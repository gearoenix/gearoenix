#include "gles2-shd-effect-forward-pbr-directional-shadow.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

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
	"uniform mat4 camera_vp;\n"
	// light uniform(s)
	"uniform mat4 light_vp_bias;\n"
	// model uniform(s)
	"uniform mat4 model_m;\n"
	// output(s)
	"varying vec3 out_pos;\n"
    "varying vec3 out_nrm;\n"
    "varying vec3 out_tng;\n"
    "varying vec3 out_btg;\n"
	"varying vec2 out_uv;\n"
	"varying vec3 out_light_pos;\n"
    // Main function
	"void main()\n"
	"{\n"
    "    vec4 pos = model_m * vec4(position, 1.0);\n"
	"    out_pos = pos.xyz;\n"
    "    out_nrm = normalize((model_m * vec4(normal, 0.0)).xyz);\n"
    "    out_tng = normalize((model_m * vec4(tangent.xyz, 0.0)).xyz);\n"
    "    out_btg = cross(out_nrm, out_tng) * tangent.w;\n"
	"    out_uv = uv;\n"
    "    vec4 light_pos = light_vp_bias * pos;\n"
	"    out_light_pos = light_pos.xyz / light_pos.w;\n"
    "    gl_Position = camera_vp * pos;\n"
	"}";

const static std::string fragment_shader_code =
	// precisions
	"precision highp   float;\n" // 1
	"precision highp   sampler2D;\n" // 2
	"precision highp   samplerCube;\n" // 3
	// constant(s)
	"#define GXPI      3.14159265359\n" // 4
	// material uniform(s)
	"uniform float     material_alpha;\n" // 5
	"uniform float     material_alpha_cutoff;\n" // 6
	"uniform float     material_metallic_factor;\n" // 7
	"uniform float     material_normal_scale;\n" // 8
	"uniform float     material_occlusion_strength;\n" // 9
	"uniform float     material_roughness_factor;\n" // 10
	"uniform sampler2D material_base_color;\n" // 11
	"uniform sampler2D material_metallic_roughness;\n" // 12
	"uniform sampler2D material_normal;\n" // 13
	"uniform sampler2D material_emissive;\n" // 14
	// scenes uniform(s)
	"uniform vec3      scene_ambient_light;\n" // 15
	"uniform vec3      scene_directional_lights_color[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n" // 16
	"uniform vec3      scene_directional_lights_direction[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n" // 17
	"uniform vec4      scene_point_lights_color_min_radius[" GX_MAX_POINT_LIGHTS_STR "];\n" // 18
	"uniform vec4      scene_point_lights_position_max_radius[" GX_MAX_POINT_LIGHTS_STR "];\n" // 19
	// directional, point
	"uniform vec2      scene_lights_count;\n" // 20
	// samples-count, radius, z-tolerance
	"uniform vec3      scene_ssao_config;\n" // 21
	// effect uniforms
	"uniform samplerCube effect_diffuse_environment;\n" // 22
	"uniform samplerCube effect_specular_environment;\n" // 23
	"uniform sampler2D effect_ambient_occlusion;\n" // 24
	"uniform sampler2D effect_shadow_map;\n" // 25
	"uniform sampler2D effect_brdflut;\n" // 26
	// light uniform(s)
	"uniform vec3      light_color;\n" // 27
	"uniform vec3      light_direction;\n" // 28
	// camera uniform(s)
	"uniform vec3      camera_position;\n" // 29
	// output(s) of vertex shader
	"varying vec3 out_pos;\n" // 30
    "varying vec3 out_nrm;\n" // 31
    "varying vec3 out_tng;\n" // 32
    "varying vec3 out_btg;\n" // 33
	"varying vec2 out_uv;\n" // 34
	"varying vec3 out_light_pos;\n" // 35
	// Normal Distribution Function Trowbridge-Reitz GGX
	"float distribution_ggx(const vec3 normal, const vec3 halfway, const float roughness) {\n" // 37
	"    float roughness2 = roughness * roughness;\n" // 38
	"    float nh = max(dot(normal, halfway), 0.0);\n" // 39
	"    float nh2 = nh * nh;\n" // 40
	"    float nom = roughness2;\n" // 41
	"    float tmpdenom = (nh2 * (roughness2 - 1.0) + 1.0);\n" // 42
	"    float denom = GXPI * tmpdenom * tmpdenom;\n" // 43
	"    return nom / denom;\n" // 44
	"}\n" // 45
	"float geometry_schlick_ggx(const float nd, const float roughness) {\n" // 46
	"    float r = roughness + 1.0;\n" // 47
	"    float k = (r * r) * (1.0 / 8.0);\n" // 48
	"    float nom = nd;\n" // 49
	"    float denom = (nd * (1.0 - k)) + k;\n" // 50
	"    return nom / denom;\n" // 51
	"}\n" // 52
	"float geometry_smith(const vec3 normal, const vec3 view, const vec3 light, const float roughness) {\n" // 53
	"    float nv = max(dot(normal, view), 0.0);\n" // 54
	"    float nl = max(dot(normal, light), 0.0);\n" // 55
	"    float ggx2 = geometry_schlick_ggx(nv, roughness);\n" // 56
	"    float ggx1 = geometry_schlick_ggx(nl, roughness);\n" // 57
	"    return ggx1 * ggx2;\n" // 58
	"}\n" // 59
	"vec3 fresnel_schlick(const float nv, const vec3 f0) {\n" // 60
	"    float inv = 1.0 - nv;\n" // 61
	"    float inv2 = inv * inv;\n" // 62
	"    float inv4 = inv2 * inv2;\n" // 63
	"    float inv5 = inv4 * inv;\n" // 64
	"    return f0 + ((1.0 - f0) * inv5);\n" // 64
	"}\n" // 65
	// cos_theta is (n dot v)
	"vec3 fresnel_schlick_roughness(const float nv, const vec3 f0, const float roughness)\n" // 66
	"{\n" // 67
	"    float inv = 1.0 - nv;\n" // 68
	"    float inv2 = inv * inv;\n" // 69
	"    float inv4 = inv2 * inv2;\n" // 70
	"    float inv5 = inv4 * inv;\n" // 71
	"    return f0 + ((max(vec3(1.0 - roughness), f0) - f0) * inv5);\n" // 72
	"}\n" // 73
	"void main()\n" // 74
	"{\n" // 75
	//   material properties
	"    vec4 tmpv4 = texture2D(material_base_color, out_uv);\n" // 76
	"    tmpv4.w *= material_alpha;\n" // 77
	"    vec4 albedo = tmpv4;\n" // 78
	"    if(albedo.w < material_alpha_cutoff) discard;\n" // 79
	"	 tmpv4.xy = texture2D(material_metallic_roughness, out_uv).xy;\n" // 80
	"    tmpv4.xy *= vec2(material_metallic_factor, material_roughness_factor);\n" // 81
	"    float metallic = tmpv4.x;\n" // 82
	"    float roughness = tmpv4.y;\n" // 83
	//   TODO: in future maybe I will add ao in here
	//   input lighting data
	"    vec3 normal = mat3(out_tng, out_btg, out_nrm) * ((texture2D(material_normal, out_uv).xyz - 0.5) * 2.0 * material_normal_scale);\n" // 84
	"    vec3 view = normalize(camera_position - out_pos);\n" // 85
	"    vec3 reflection = reflect(-view, normal);\n" // 86
	//   calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	//   of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	//   TODO: in future I must bring builder fresnel factor 0 (the hard coded 0.4) from matrial uniform data
	"    vec3 f0 = mix(vec3(0.04), albedo.xyz, metallic);\n" // 87
	//   reflectance equation
	"    vec3 lo = vec3(0.0);\n" // 88
	//   computing point lights
	"    for (int i = 0; i < " GX_MAX_POINT_LIGHTS_STR "; ++i)\n" // 89
	"    {\n" // 90
	//       calculate per-light radiance
	"        vec3 light_vec = scene_point_lights_position_max_radius[i].xyz - out_pos;\n" // 91
	//       TODO: in future consider max and min radius
	"        float distance = length(light_vec);\n" // 92
	"        float distance_inv = 1.0 / distance;\n" // 93
	"        vec3 light_direction = light_vec * distance_inv;\n" // 94
	"        vec3 half_vec = normalize(view + light_direction);\n" // 95
	"        float attenuation = distance_inv * distance_inv;\n" // 96
	"        vec3 radiance = scene_point_lights_color_min_radius[i].xyz * attenuation;\n" // 97
	//       Cook-Torrance BRDF
	"        float ndf = distribution_ggx(normal, half_vec, roughness);\n" // 98
	"        float geo = geometry_smith(normal, view, light_direction, roughness);\n" // 99
	"        vec3 frsn = fresnel_schlick(max(dot(half_vec, view), 0.0), f0);\n" // 100
	"        vec3 nominator = ndf * geo * frsn;\n" // 101
	//       0.001 to prevent divide by zero.
	"        float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light_direction), 0.0) + 0.001;\n" // 102
	"        vec3 specular = nominator / denominator;\n" // 103
	//       kS is equal to Fresnel
	"        vec3 ks = frsn;\n" // 104
	//       for energy conservation, the diffuse and specular light can't
	//       be above 1.0 (unless the surface emits light); to preserve this
	//       relationship the diffuse component (kD) should equal 1.0 - kS.
	//       multiply kD by the inverse metalness such that only non-metals
	//       have diffuse lighting, or a linear blend if partly metal (pure metals
	//       have no diffuse light).
	"        vec3 kd = (vec3(1.0) - ks) * (1.0 - metallic);\n" // 105
	//       scale light by NdotL
	"        float nl = max(dot(normal, light_direction), 0.0);\n" // 106
	//       add to outgoing radiance Lo
	//       note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	"        lo += (kd * albedo / GXPI + specular) * radiance * nl;\n" // 107
	"    }\n" // 108
	//   ambient lighting (we now use IBL as the ambient term)
	"    vec3 frsn = fresnel_schlick_roughness(max(dot(normal, view), 0.0), f0, roughness);\n" // 109
	"    vec3 ks = frsn;\n" // 110
	"    vec3 kd = (1.0 - ks) * (1.0 - metallic);\n" // 111
	"    vec3 irradiance = textureCube(effect_diffuse_environment, normal).rgb;\n" // 112
	"    vec3 diffuse = irradiance * albedo;\n" // 113
	//   sample both the pre-filter map and the BRDF lut and combine them together as per
	//   the Split-Sum approximation to get the IBL specular part.
	"    float MAX_REFLECTION_LOD = 4.0;\n" // 114
	"    vec3 prefiltered_color = textureCube(effect_specular_environment, reflection, roughness * MAX_REFLECTION_LOD).rgb;\n" // 115
	"    vec2 brdf = texture2D(effect_brdflut, vec2(max(dot(normal, view), 0.0), roughness)).rg;\n" // 116
	"    vec3 specular = prefiltered_color * (frsn * brdf.x + brdf.y);\n" // 117
	//   TODO: add ambient occlusion (* ao);
	"    vec3 ambient = kd * diffuse + specular;\n" // 118
	"    tmpv4.xyz = ambient + lo;\n" // 119
	//   HDR tonemapping
	"    tmpv4.xyz = tmpv4.xyz / (tmpv4.xyz + vec3(1.0));\n" // 120
	//   gamma correct
	"    tmpv4.xyz = pow(tmpv4.xyz, vec3(1.0 / 2.2));\n" // 121
	//   TODO don't forget gamma correction it can be part of scene uniform data
	"    gl_FragColor = vec4(tmpv4.xyz, albedo.w);\n" // 122
	"}"; // 123

gearoenix::gles2::shader::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: Shader(e, c)
{
	e->get_function_loader()->load([this] {
		set_vertex_shader(vertex_shader_code);
		set_fragment_shader(fragment_shader_code);
		link();
		GX_GLES2_THIS_GET_UNIFORM_F(camera_position);
		GX_GLES2_THIS_GET_UNIFORM_F(camera_vp);
		// TODO
		//GX_GLES2_THIS_GET_UNIFORM_F(effect_ambient_occlusion);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_brdflut);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_diffuse_environment);
		// TODO
		// GX_GLES2_THIS_GET_UNIFORM_F(effect_shadow_map);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_specular_environment);
		/*GX_GLES2_THIS_GET_UNIFORM_F(light_color);
		GX_GLES2_THIS_GET_UNIFORM_F(light_direction);
		GX_GLES2_THIS_GET_UNIFORM_F(light_vp_bias);*/
		GX_GLES2_THIS_GET_UNIFORM_F(material_alpha);
		GX_GLES2_THIS_GET_UNIFORM_F(material_alpha_cutoff);
		GX_GLES2_THIS_GET_UNIFORM_F(material_base_color);
		//GX_GLES2_THIS_GET_UNIFORM_F(material_emissive);
		GX_GLES2_THIS_GET_UNIFORM_F(material_metallic_factor);
		GX_GLES2_THIS_GET_UNIFORM_F(material_metallic_roughness);
		GX_GLES2_THIS_GET_UNIFORM_F(material_normal);
		GX_GLES2_THIS_GET_UNIFORM_F(material_normal_scale);
		//GX_GLES2_THIS_GET_UNIFORM_F(material_occlusion_strength);
		GX_GLES2_THIS_GET_UNIFORM_F(material_roughness_factor);
		GX_GLES2_THIS_GET_UNIFORM_F(model_m);
		// GX_GLES2_THIS_GET_UNIFORM_F(scene_ambient_light);
		// GX_GLES2_THIS_GET_UNIFORM_F(scene_directional_lights_color);
		// GX_GLES2_THIS_GET_UNIFORM_F(scene_directional_lights_direction);
		// GX_GLES2_THIS_GET_UNIFORM_F(scene_lights_count);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_point_lights_color_min_radius);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_point_lights_position_max_radius);
		// GX_GLES2_THIS_GET_UNIFORM_F(scene_ssao_config);
	});
	gl::sint texture_index = 0;
	material_base_color_index = texture_index;
	++texture_index;
	material_metallic_roughness_index = texture_index;
	++texture_index;
	material_normal_index = texture_index;
	++texture_index;
	material_emissive_index = texture_index;
	++texture_index;
	effect_diffuse_environment_index = texture_index;
	++texture_index;
	effect_specular_environment_index = texture_index;
	++texture_index;
	effect_ambient_occlusion_index = texture_index;
	++texture_index;
	effect_shadow_map_index = texture_index;
	++texture_index;
	effect_brdflut_index = texture_index;
}

gearoenix::gles2::shader::ForwardPbrDirectionalShadow::~ForwardPbrDirectionalShadow()
{
}

void gearoenix::gles2::shader::ForwardPbrDirectionalShadow::bind() const
{
	gl::Loader::uniform1i(material_base_color, material_base_color_index);
	gl::Loader::uniform1i(material_metallic_roughness, material_metallic_roughness_index);
	gl::Loader::uniform1i(material_normal, material_normal_index);
	gl::Loader::uniform1i(material_emissive, material_emissive_index);
	gl::Loader::uniform1i(effect_diffuse_environment, effect_diffuse_environment_index);
	gl::Loader::uniform1i(effect_specular_environment, effect_specular_environment_index);
	gl::Loader::uniform1i(effect_ambient_occlusion, effect_ambient_occlusion_index);
	gl::Loader::uniform1i(effect_shadow_map, effect_shadow_map_index);
	gl::Loader::uniform1i(effect_brdflut, effect_brdflut_index);
}

#endif
