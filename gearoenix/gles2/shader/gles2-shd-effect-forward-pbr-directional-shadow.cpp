#include "gles2-shd-effect-forward-pbr-directional-shadow.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
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
	"precision highp   float;\n"
	"precision highp   sampler2D;\n"
	"precision highp   samplerCube;\n"
	// constant(s)
	"#define GXPI      3.14159265359\n"
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
	"uniform vec3      scene_ambient_light;\n"
	"uniform vec3      scene_directional_lights_color[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n"
	"uniform vec3      scene_directional_lights_direction[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n"
	"uniform vec4      scene_point_lights_color_min_radius[" GX_MAX_POINT_LIGHTS_STR "];\n"
	"uniform vec4      scene_point_lights_position_max_radius[" GX_MAX_POINT_LIGHTS_STR "];\n"
	// directional, point
	"uniform vec2      scene_lights_count;\n"
	// samples-count, radius, z-tolerance
	"uniform vec3      scene_ssao_config;\n"
	// effect uniforms
	"uniform samplerCube effect_diffuse_environment;\n"
	"uniform samplerCube effect_specular_environment;\n"
	"uniform sampler2D effect_ambient_occlusion;\n"
	"uniform sampler2D effect_shadow_map;\n"
	"uniform sampler2D effect_brdflut;\n"
	// light uniform(s)
	"uniform vec3      light_color;\n"
	"uniform vec3      light_direction;\n"
	// camera uniform(s)
	"uniform vec3      camera_position;\n"
	// output(s) of vertex shader
	"varying vec3 out_pos;\n"
	"varying mat3 out_nrm;\n"
	"varying mat3 out_tng;\n"
	"varying mat3 out_tbn;\n"
	"varying vec2 out_uv;\n"
	"varying vec3 out_light_pos;\n"
	// Normal Distribution Function Trowbridge-Reitz GGX
	"float distribution_ggx(const vec3 normal, const vec3 halfway, const float roughness) {\n"
	"    const float roughness2 = roughness * roughness;\n"
	"    const float nh = max(dot(normal, halfway), 0.0);\n"
	"    const float nh2 = nh * nh;\n"
	"    const float nom = roughness2;\n"
	"    const float tmpdenom = (nh2 * (roughness2 - 1.0) + 1.0);\n"
	"    const float denom = GXPI * tmpdenom * tmpdenom;\n"
	"    return nom / denom;\n"
	"}\n"
	"float geometry_schlick_ggx(const float nd, const float roughness) {\n"
	"    const float r = roughness + 1.0;\n"
	"    const float k = (r * r) * (1.0 / 8.0);\n"
	"    const float nom = nd;\n"
	"    const float denom = (nd * (1.0 - k)) + k;\n"
	"    return nom / denom;\n"
	"}\n"
	"float geometry_smith(const vec3 normal, const vec3 view, const vec3 light, const float roughness) {\n"
	"    const float nv = max(dot(normal, view), 0.0);\n"
	"    const float nl = max(dot(normal, light), 0.0);\n"
	"    const float ggx2 = GFSCHGGX(nv, roughness);\n"
	"    const float ggx1 = GFSCHGGX(nl, roughness);\n"
	"    return ggx1 * ggx2;\n"
	"}\n"
	"vec3 fresnel_schlick(const float nv, const vec3 f0) {\n"
	"    const float inv = 1.0 - nv;\n"
	"    const float inv2 = inv * inv;\n"
	"    const float inv4 = inv2 * inv2;\n"
	"    const float inv5 = inv4 * inv;\n"
	"    return f0 + ((1.0 - f0) * inv5);\n"
	"}\n"
	// cos_theta is (n dot v)
	"vec3 fresnel_schlick_roughness(const float nv, const vec3 f0, const float roughness)\n"
	"{\n"
	"    const float inv = 1.0 - nv;\n"
	"    const float inv2 = inv * inv;\n"
	"    const float inv4 = inv2 * inv2;\n"
	"    const float inv5 = inv4 * inv;\n"
	"    return f0 + ((max(vec3(1.0 - roughness), f0) - f0) * inv5);\n"
	"}\n"
	"void main()\n"
	"{\n"
	//   material properties
	"    vec4 tmpv4 = texture2D(material_base_color, out_uv);\n"
	"    tmpv4.w *= material_alpha;\n"
	"    const vec4 albedo = tmpv4;\n"
	"    if(albedo.w < material_alpha_cutoff) discard;\n"
	"	 tmpv4.xy = texture2D(material_metallic_roughness, out_uv).xy;\n"
	"    tmpv4.xy *= vec2(material_metallic_factor, material_roughness_factor);\n"
	"    const float metallic = tmpv4.x;\n"
	"    const float roughness = tmpv4.y;\n"
	//   TODO: in future maybe I will add ao in here
	//   input lighting data
	"    const vec3 normal = mat3(tng, btg, nrm) * ((texture2D(material_normal, out_uv).xyz - 0.5) * 2.0 * material_normal_scale);\n"
	"    const vec3 view = normalize(camera_position - out_pos);\n"
	"    const vec3 reflection = reflect(-view, normal);\n"
	//   calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	//   of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	//   TODO: in future I must bring builder fresnel factor 0 (the hard coded 0.4) from matrial uniform data
	"    const vec3 f0 = mix(vec3(0.04), albedo.xyz, metallic);\n"
	//   reflectance equation
	"    vec3 lo = vec3(0.0);\n"
	//   computing point lights
	"    for (int i = 0; i < " GX_MAX_POINT_LIGHTS_STR "; ++i)\n"
	"    {\n"
	//       calculate per-light radiance
	"        const vec3 light_vec = scene_point_lights_position_max_radius[i].xyz - out_pos;\n"
	//       TODO: in future consider max and min radius
	"        const float distance = length(light_vec);\n"
	"        const float distance_inv = 1.0 / distance;\n"
	"        const vec3 light_direction = light_vec * distance_inv;\n"
	"        const vec3 half_vec = normalize(view + light);\n"
	"        const float attenuation = distance * distance;\n"
	"        const vec3 radiance = scene_point_lights_position_max_radius[i].xyz * attenuation;\n"
	//       Cook-Torrance BRDF
	"        const float ndf = distribution_ggx(normal, half_vec, roughness);\n"
	"        const float geo = geometry_smith(normal, view, light_direction, roughness);\n"
	"        const vec3 frsn = fresnel_schlick(max(dot(half_vec, view), 0.0), f0);\n"
	"        const vec3 nominator = ndf * geo * frsn;\n"
	//       0.001 to prevent divide by zero.
	"        const float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light_direction), 0.0) + 0.001;\n"
	"        const vec3 specular = nominator / denominator;\n"
	//       kS is equal to Fresnel
	"        const vec3 ks = frsn;\n"
	//       for energy conservation, the diffuse and specular light can't
	//       be above 1.0 (unless the surface emits light); to preserve this
	//       relationship the diffuse component (kD) should equal 1.0 - kS.
	//       multiply kD by the inverse metalness such that only non-metals
	//       have diffuse lighting, or a linear blend if partly metal (pure metals
	//       have no diffuse light).
	"        const vec3 kd = (vec3(1.0) - ks) * (1.0 - metallic);\n"
	//       scale light by NdotL
	"        const float nl = max(dot(normal, light_direction), 0.0);\n"
	//       add to outgoing radiance Lo
	//       note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	"        lo += (kd * albedo / GXPI + specular) * radiance * nl;\n"
	"    }\n"
	//   ambient lighting (we now use IBL as the ambient term)
	"    const vec3 frsn = fresnel_schlick_roughness(max(dot(normal, view), 0.0), f0, roughness);\n"
	"    const vec3 ks = frsn;\n"
	"    const vec3 kd = (1.0 - ks) * (1.0 - metallic);\n"
	"    const vec3 irradiance = textureCube(effect_diffuse_environment, normal).rgb;\n"
	"    const vec3 diffuse = irradiance * albedo;\n"
	//   sample both the pre-filter map and the BRDF lut and combine them together as per
	//   the Split-Sum approximation to get the IBL specular part.
	"    const float MAX_REFLECTION_LOD = 4.0;\n"
	"    const vec3 prefiltered_color = textureCube(effect_specular_environment, reflection, roughness * MAX_REFLECTION_LOD).rgb;\n"
	"    const vec2 brdf = texture2D(effect_brdflut, vec2(max(dot(normal, view), 0.0), roughness)).rg;\n"
	"    const vec3 specular = prefiltered_color * (frsn * brdf.x + brdf.y);\n"
	//   TODO: add ambient occlusion (* ao);
	"    const vec3 ambient = kd * diffuse + specular;\n"
	"    tmpv4.xyz = ambient + lo;\n"
	//   HDR tonemapping
	"    tmpv4.xyz = tmpv4.xyz / (tmpv4.xyz + vec3(1.0));\n"
	//   gamma correct
	"    color = pow(color, vec3(1.0 / 2.2));\n"
	//   TODO don't forget gamma correction it can be part of scene uniform data
	"    gl_FragColor = vec4(color, 1.0);\n"
	"}";

gearoenix::gles2::shader::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: Shader(e, c)
{
	e->get_function_loader()->load([this] {
		set_vertex_shader(vertex_shader_code);
		set_fragment_shader(fragment_shader_code);
		link();
		GX_GLES2_THIS_GET_UNIFORM_F(camera_position);
		GX_GLES2_THIS_GET_UNIFORM_F(camera_vp);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_ambient_occlusion);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_brdflut);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_diffuse_environment);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_shadow_map);
		GX_GLES2_THIS_GET_UNIFORM_F(effect_specular_environment);
		GX_GLES2_THIS_GET_UNIFORM_F(light_color);
		GX_GLES2_THIS_GET_UNIFORM_F(light_direction);
		GX_GLES2_THIS_GET_UNIFORM_F(light_vp_bias);
		GX_GLES2_THIS_GET_UNIFORM_F(material_alpha);
		GX_GLES2_THIS_GET_UNIFORM_F(material_alpha_cutoff);
		GX_GLES2_THIS_GET_UNIFORM_F(material_base_color);
		GX_GLES2_THIS_GET_UNIFORM_F(material_emissive);
		GX_GLES2_THIS_GET_UNIFORM_F(material_metallic_factor);
		GX_GLES2_THIS_GET_UNIFORM_F(material_metallic_roughness);
		GX_GLES2_THIS_GET_UNIFORM_F(material_normal);
		GX_GLES2_THIS_GET_UNIFORM_F(material_normal_scale);
		GX_GLES2_THIS_GET_UNIFORM_F(material_occlusion_strength);
		GX_GLES2_THIS_GET_UNIFORM_F(material_roughness_factor);
		GX_GLES2_THIS_GET_UNIFORM_F(model_m);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_ambient_light);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_directional_lights_color);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_directional_lights_direction);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_lights_count);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_point_lights_color_min_radius);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_point_lights_position_max_radius);
		GX_GLES2_THIS_GET_UNIFORM_F(scene_ssao_config);
	});
	GLint texture_index = 0;
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

#endif
