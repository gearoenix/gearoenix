#include "gx-gl-shd-forward-pbr.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"
#include <sstream>
#include <string>

gearoenix::gl::shader::ForwardPbr::ForwardPbr(
    Engine& e,
    const std::size_t directional_lights_count,
    const std::size_t shadow_casters_directional_lights_count,
    const std::size_t bones_count)
    : Shader(e)
    , directional_lights_count(static_cast<sizei>(directional_lights_count))
    , shadow_caster_directional_light_shadow_map_indices(shadow_casters_directional_lights_count)
    , bones_matrices_count(static_cast<sizei>(bones_count * 2))
{
    // Vertex Shader -----------------------------------------------------------------------------------
    std::stringstream vs;
    vs << "#version 300 es\n";
    vs << "\n";
    vs << "#define GX_PI 3.141592653589793238\n";
    vs << "\n";
    vs << "precision highp float;\n";
    vs << "precision highp int;\n";
    vs << "\n";
    vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION << ") in vec3 position;\n";
    vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL << ") in vec3 normal;\n";
    vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT << ") in vec4 tangent;\n";
    vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV << ") in vec2 uv;\n";
    if (0 < bones_count) {
        vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_WEIGHTS << ") in vec4 bones_weight;\n";
        vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_INDICES << ") in vec4 bones_indices;\n";
        vs << "\n";
        vs << "uniform mat4 bones_m_inv_m[" << (bones_count * 2) << "];\n";
    } else {
        vs << "\n";
        vs << "uniform mat4 m;\n";
        vs << "uniform mat4 inv_m;\n";
    }
    vs << "\n";
    vs << "uniform mat4 vp;\n";
    vs << "\n";
    vs << "out vec3 out_pos;\n";
    vs << "out vec3 out_nrm;\n";
    vs << "out vec3 out_tng;\n";
    vs << "out vec3 out_btg;\n";
    vs << "out vec2 out_uv;\n";
    vs << "\n";
    vs << "void main() {\n";
    vs << "    out_uv = uv;\n";
    if (0 < bones_count) {
        vs << "    ivec4 bone_index = ivec4(bones_indices) * 2;\n";
        vs << "    mat4 m = (bones_m_inv_m[bone_index.x] * bones_weight.x) +\n";
        vs << "        (bones_m_inv_m[bone_index.y] * bones_weight.y) +\n";
        vs << "        (bones_m_inv_m[bone_index.z] * bones_weight.z) +\n";
        vs << "        (bones_m_inv_m[bone_index.w] * bones_weight.w);\n";
        vs << "    bone_index += 1;\n";
        vs << "    mat4 inv_m = (bones_m_inv_m[bone_index.x] * bones_weight.x) +\n";
        vs << "        (bones_m_inv_m[bone_index.y] * bones_weight.y) +\n";
        vs << "        (bones_m_inv_m[bone_index.z] * bones_weight.z) +\n";
        vs << "        (bones_m_inv_m[bone_index.w] * bones_weight.w);\n";
    }
    vs << "    vec4 pos = m * vec4(position, 1.0);\n";
    vs << "    out_nrm = (inv_m * vec4(normal, 0.0)).xyz;\n";
    vs << "    out_tng = (inv_m * vec4(tangent.xyz, 0.0)).xyz;\n";
    vs << "    out_pos = pos.xyz;\n";
    vs << "    out_btg = cross(out_nrm, out_tng) * tangent.w;\n";
    vs << "    gl_Position = vp * pos;\n";
    vs << "}\n";
    set_vertex_shader(vs.str());
    // Fragment Shader ---------------------------------------------------------------------------------
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "#define GX_PI 3.141592653589793238\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "precision highp int;\n";
    fs << "precision highp sampler2D;\n";
    fs << "precision highp samplerCube;\n";
    fs << "precision highp sampler2DShadow;\n";
    fs << "\n";
    fs << "uniform vec4 camera_position_reserved;\n";
    fs << "uniform vec4 albedo_factor;\n";
    fs << "uniform vec4 normal_metallic_factor;\n";
    fs << "uniform vec4 emission_roughness_factor;\n";
    fs << "uniform vec4 alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved;\n";
    fs << "\n";
    if (directional_lights_count > 0) {
        fs << "uniform vec3 directional_light_direction[" << directional_lights_count << "];\n";
        fs << "uniform vec3 directional_light_colour[" << directional_lights_count << "];\n";
    }
    if (shadow_casters_directional_lights_count > 0) {
        fs << "uniform vec3 shadow_caster_directional_light_direction[" << shadow_casters_directional_lights_count << "];\n";
        fs << "uniform vec3 shadow_caster_directional_light_colour[" << shadow_casters_directional_lights_count << "];\n";
        fs << "uniform mat4 shadow_caster_directional_light_normalised_vp[" << shadow_casters_directional_lights_count << "];\n";
        fs << "\n";
        fs << "uniform sampler2DShadow shadow_caster_directional_light_shadow_map[" << shadow_casters_directional_lights_count << "];\n";
    }
    fs << "\n";
    fs << "uniform sampler2D albedo;\n";
    fs << "uniform sampler2D normal;\n";
    fs << "uniform sampler2D emission;\n";
    fs << "uniform sampler2D metallic_roughness;\n";
    fs << "uniform sampler2D occlusion;\n";
    fs << "uniform sampler2D brdflut;\n";
    fs << "\n";
    fs << "uniform samplerCube irradiance;\n";
    fs << "uniform samplerCube radiance;\n";
    fs << "\n";
    fs << "in vec3 out_pos;\n";
    fs << "in vec3 out_nrm;\n";
    fs << "in vec3 out_tng;\n";
    fs << "in vec3 out_btg;\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "layout(location = 0) out vec4 frag_out;\n";
    fs << "\n";
    fs << "float distribution_ggx(const float normal_dot_half, const float roughness) {\n";
    fs << "    float a = normal_dot_half * roughness;\n";
    fs << "    float k = roughness / (1.0 - normal_dot_half * normal_dot_half + a * a);\n";
    fs << "    return k * k * (1.0 / GX_PI);\n";
    fs << "}\n";
    fs << "\n";
    fs << "float geometry_smith(const float normal_dot_light, const float normal_dot_view, const float roughness) {\n";
    fs << "    float v = normal_dot_light * (normal_dot_view * (1.0 - roughness) + roughness);\n";
    fs << "    float l = normal_dot_view * (normal_dot_light * (1.0 - roughness) + roughness);\n";
    fs << "    return 0.5 / (v + l);\n";
    fs << "}\n";
    fs << "\n";
    fs << "vec3 fresnel_schlick(const float normal_dot_half, const vec3 f0, const float f90) {\n";
    fs << "    return f0 + (vec3(f90) - f0) * pow(1.0 - normal_dot_half, 5.0);\n";
    fs << "}\n";
    fs << "\n";
    fs << "vec3 compute_light(\n";
    fs << "        const vec3 light_direction,\n";
    fs << "        const vec3 light_color,\n";
    fs << "        const vec3 view,\n";
    fs << "        const vec3 normal,\n";
    fs << "        const float normal_dot_view,\n";
    fs << "        const float roughness,\n";
    fs << "        const float metallic,\n";
    fs << "        const vec3 f0,\n";
    fs << "        const float f90,\n";
    fs << "        const vec3 albedo) {\n";
    fs << "    float normal_dot_light = max(dot(normal, light_direction), 0.0001);\n";
    fs << "    vec3 half_vec = normalize(view + light_direction);\n";
    fs << "    float normal_dot_half = max(dot(normal, half_vec), 0.0001);\n";
    fs << "    float ndf = distribution_ggx(normal_dot_half, roughness);\n";
    fs << "    float geo = geometry_smith(normal_dot_light, normal_dot_view, roughness);\n";
    fs << "    vec3 fresnel = fresnel_schlick(dot(half_vec, view), f0, f90);\n";
    fs << "    vec3 specular = (ndf * geo) * fresnel;\n";
    fs << "    vec3 diffuse = albedo * ( 1.0 / GX_PI);\n";
    fs << "    vec3 kd = (vec3(1.0) - fresnel) * (1.0 - metallic);\n";
    fs << "    return (diffuse * kd + specular) * light_color * normal_dot_light;\n";
    fs << "}\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "\n";
    fs << "    vec4 alb = texture(albedo, out_uv) * albedo_factor;\n";
    fs << "    if(alb.w <= alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.x) discard;\n";
    fs << "    vec2 mtr = texture(metallic_roughness, out_uv).xy * vec2(normal_metallic_factor.w, emission_roughness_factor.w);\n";
    fs << "    float metallic = clamp(mtr.x, 0.0001, 0.9999);\n";
    fs << "    float roughness = clamp(mtr.y * mtr.y, 0.0001, 0.9999);\n";
    fs << "    float ao = texture(occlusion, out_uv).x * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.y;\n";
    fs << "    vec3 nrm = normalize(mat3(out_tng, out_btg, out_nrm) * ((texture(normal, out_uv).xyz * 2.0 - 1.0) * normal_metallic_factor.xyz));\n";
    fs << "    vec3 ems = texture(emission, out_uv).xyz * emission_roughness_factor.xyz;\n";
    fs << "    vec3 f0 = mix(vec3(0.04), alb.xyz, metallic);\n";
    fs << "    float f90 = clamp(dot(f0, vec3(50.0 * 0.33)), 0.0001, 1.0);\n";
    fs << "    vec3 eye = normalize(out_pos - camera_position_reserved.xyz);\n";
    fs << "    vec3 reflection = reflect(eye, nrm);\n";
    fs << "    float normal_dot_view = max(-dot(nrm, eye), 0.0001);\n";
    fs << "    vec3 fresnel = fresnel_schlick(normal_dot_view, f0, f90);\n";
    fs << "    vec3 illumination = vec3(0.0001);\n";
    fs << "\n";
    for (int dir_i = 0; dir_i < static_cast<int>(directional_lights_count); ++dir_i) {
        fs << "    {\n";
        fs << "        illumination += compute_light(\n";
        fs << "            directional_light_direction[" << dir_i << "],\n";
        fs << "            directional_light_colour[" << dir_i << "],\n";
        fs << "            -eye,\n";
        fs << "            nrm,\n";
        fs << "            normal_dot_view,\n";
        fs << "            roughness,\n";
        fs << "            metallic,\n";
        fs << "            f0,\n";
        fs << "            f90,\n";
        fs << "            alb.xyz);\n";
        fs << "    }\n";
    }
    for (int dir_i = 0; dir_i < static_cast<int>(shadow_casters_directional_lights_count); ++dir_i) {
        fs << "    {\n";
        fs << "        float normal_dot_light = max(dot(nrm, shadow_caster_directional_light_direction[" << dir_i << "]), 0.00001);\n";
        fs << "        float shadow_bias = clamp(sqrt((0.000025 / (normal_dot_light * normal_dot_light)) - 0.000025), 0.001, 0.02);\n";
        fs << "        vec4 light_uv_depth = shadow_caster_directional_light_normalised_vp[" << dir_i << "] * vec4(out_pos, 1.0);\n";
        fs << "        light_uv_depth.xyz /= light_uv_depth.w;\n";
        fs << "        light_uv_depth.xyz *= 0.5;\n";
        fs << "        light_uv_depth.xyz += 0.5;\n";
        fs << "        light_uv_depth.z -= shadow_bias;\n";
        fs << "        vec3 uv_bounds = step(vec3(0.0), light_uv_depth.xyz) * step(light_uv_depth.xyz, vec3(1.0));\n";
        fs << "        float shadow_w = uv_bounds.x * uv_bounds.y * uv_bounds.z;\n";
        fs << "        shadow_w *= 1.0 - texture(shadow_caster_directional_light_shadow_map[" << dir_i << "], light_uv_depth.xyz, 0.0);\n";
        fs << "        float light_w = 1.0 - shadow_w;\n";
        fs << "        illumination += light_w * compute_light(\n";
        fs << "            shadow_caster_directional_light_direction[" << dir_i << "],\n";
        fs << "            shadow_caster_directional_light_colour[" << dir_i << "],\n";
        fs << "            -eye,\n";
        fs << "            nrm,\n";
        fs << "            normal_dot_view,\n";
        fs << "            roughness,\n";
        fs << "            metallic,\n";
        fs << "            f0,\n";
        fs << "            f90,\n";
        fs << "            alb.xyz);\n";
        fs << "    }\n";
    }
    fs << "\n";
    fs << "    vec3 irr = texture(irradiance, nrm).xyz * 1.1;\n";
    fs << "    vec3 rad = textureLod(radiance, reflection, mtr.y * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.z).xyz;\n";
    fs << "    vec3 ambient = irr * alb.xyz * (vec3(f90) - fresnel) * (f90 - metallic);\n";
    fs << "    vec2 brdf = texture(brdflut, vec2(normal_dot_view, roughness)).rg;\n";
    fs << "    ambient += rad * ((fresnel * brdf.x) + (f90 * brdf.y));\n";
    fs << "    ambient *= ao;\n";
    fs << "    vec3 frag_colour = ambient + ems + illumination;\n";
    fs << "    frag_out = vec4(frag_colour, 1.0);\n";
    fs << "}\n";
    set_fragment_shader(fs.str());

    link();

    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(vp);
    GX_GL_THIS_GET_UNIFORM(camera_position_reserved);
    GX_GL_THIS_GET_UNIFORM(albedo_factor);
    GX_GL_THIS_GET_UNIFORM(normal_metallic_factor);
    GX_GL_THIS_GET_UNIFORM(emission_roughness_factor);
    GX_GL_THIS_GET_UNIFORM(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(metallic_roughness);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(occlusion);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(irradiance);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(radiance);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(brdflut);

    if (directional_lights_count > 0) {
        GX_GL_THIS_GET_UNIFORM(directional_light_direction);
        GX_GL_THIS_GET_UNIFORM(directional_light_colour);
    }

    if (shadow_casters_directional_lights_count > 0) {
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_direction);
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_colour);
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_normalised_vp);
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_shadow_map);
        GX_GL_THIS_GET_UNIFORM_TEXTURE_ARRAY(shadow_caster_directional_light_shadow_map);
    }

    if (bones_count > 0) {
        GX_GL_THIS_GET_UNIFORM(bones_m_inv_m);
    } else {
        GX_GL_THIS_GET_UNIFORM(m);
        GX_GL_THIS_GET_UNIFORM(inv_m);
    }
}

gearoenix::gl::shader::ForwardPbr::ForwardPbr(ForwardPbr&&) noexcept = default;

gearoenix::gl::shader::ForwardPbr::~ForwardPbr() = default;

void gearoenix::gl::shader::ForwardPbr::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(metallic_roughness);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(occlusion);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(irradiance);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(radiance);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(brdflut);
    GX_GL_SHADER_SET_TEXTURE_INDEX_DYNAMIC_ARRAY_UNIFORM(shadow_caster_directional_light_shadow_map);
}

void gearoenix::gl::shader::ForwardPbr::set_shadow_caster_directional_light_normalised_vp_data(const void* const data)
{
    glUniformMatrix4fv(
        shadow_caster_directional_light_normalised_vp,
        static_cast<sizei>(shadow_caster_directional_light_shadow_map_indices.size()),
        GL_FALSE,
        reinterpret_cast<const float*>(data));
}

void gearoenix::gl::shader::ForwardPbr::set_directional_light_direction_data(const void* const data)
{
    glUniform3fv(
        directional_light_direction,
        directional_lights_count,
        reinterpret_cast<const float*>(data));
}

void gearoenix::gl::shader::ForwardPbr::set_directional_light_colour_data(const void* const data)
{
    glUniform3fv(
        directional_light_colour,
        directional_lights_count,
        reinterpret_cast<const float*>(data));
}

void gearoenix::gl::shader::ForwardPbr::set_shadow_caster_directional_light_direction_data(const void* const data)
{
    glUniform3fv(
        shadow_caster_directional_light_direction,
        static_cast<sizei>(shadow_caster_directional_light_shadow_map_indices.size()),
        reinterpret_cast<const float*>(data));
}

void gearoenix::gl::shader::ForwardPbr::set_shadow_caster_directional_light_colour_data(const void* const data)
{
    glUniform3fv(
        shadow_caster_directional_light_colour,
        static_cast<sizei>(shadow_caster_directional_light_shadow_map_indices.size()),
        reinterpret_cast<const float*>(data));
}

const gearoenix::gl::sint* gearoenix::gl::shader::ForwardPbr::get_shadow_caster_directional_light_shadow_map_indices() const
{
    if (shadow_caster_directional_light_shadow_map_indices.empty())
        return nullptr;
    return shadow_caster_directional_light_shadow_map_indices.data();
}

void gearoenix::gl::shader::ForwardPbr::set_bones_m_inv_m_data(const void* const data)
{
    glUniformMatrix4fv(
        bones_m_inv_m,
        bones_matrices_count,
        GL_FALSE,
        reinterpret_cast<const float*>(data));
}

#endif
