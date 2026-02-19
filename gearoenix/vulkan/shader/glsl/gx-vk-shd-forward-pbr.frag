#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_nrm;
layout(location = 2) in vec3 in_tng;
layout(location = 3) in vec3 in_btg;
layout(location = 4) in vec2 in_uv;

layout(location = 0) out vec4 frag_out;

// ========== PBR Functions ==========

// GGX/Trowbridge-Reitz normal distribution function
float distribution_ggx(const float normal_dot_half, const float roughness) {
    float a = normal_dot_half * roughness;
    float k = roughness / (1.0 - normal_dot_half * normal_dot_half + a * a);
    return k * k * (1.0 / GX_PI);
}

// Height-correlated Smith geometry function (optimized version)
float geometry_smith(const float normal_dot_light, const float normal_dot_view, const float roughness) {
    float v = normal_dot_light * (normal_dot_view * (1.0 - roughness) + roughness);
    float l = normal_dot_view * (normal_dot_light * (1.0 - roughness) + roughness);
    return 0.5 / max(v + l, 0.0001);
}

// Schlick's approximation for Fresnel reflectance
vec3 fresnel_schlick(const float cos_theta, const vec3 f0, const float f90) {
    return f0 + (vec3(f90) - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

// Compute direct lighting contribution for a single light
vec3 compute_light(
        const vec3 light_direction,
        const vec3 light_color,
        const vec3 view,
        const vec3 normal,
        const float normal_dot_view,
        const float roughness,
        const float metallic,
        const vec3 f0,
        const float f90,
        const vec3 albedo) {

    float normal_dot_light = max(dot(normal, light_direction), 0.0001);
    vec3 half_vec = normalize(view + light_direction);
    float normal_dot_half = max(dot(normal, half_vec), 0.0001);
    float half_dot_view = max(dot(half_vec, view), 0.0001);

    // Cook-Torrance specular BRDF
    float ndf = distribution_ggx(normal_dot_half, roughness);
    float geo = geometry_smith(normal_dot_light, normal_dot_view, roughness);
    vec3 fresnel = fresnel_schlick(half_dot_view, f0, f90);
    vec3 specular = (ndf * geo) * fresnel;

    // Lambertian diffuse BRDF
    vec3 diffuse = albedo * (1.0 / GX_PI);

    // Energy conservation: diffuse is reduced by metallic and fresnel
    vec3 kd = (vec3(1.0) - fresnel) * (1.0 - metallic);

    return (diffuse * kd + specular) * light_color * normal_dot_light;
}

void main() {
    GxShaderDataScene scene = scenes[pc.scene_index];
    GxShaderDataCamera camera = cameras[pc.camera_index];
    GxShaderDataModel model = models[pc.model_index];
    GxShaderDataMaterial material = materials[pc.material_index];

    // Albedo
    vec4 alb = texture(sampler2D(textures_2d[nonuniformEXT(material.albedo_texture_index)], samplers[nonuniformEXT(material.albedo_sampler_index)]), in_uv) * material.albedo_factor;

    // Alpha test
    if (alb.a <= material.alpha_cutoff_occlusion_strength_reserved.x) {
        discard;
    }

    vec2 mtr = texture(sampler2D(textures_2d[nonuniformEXT(material.metallic_roughness_texture_index)], samplers[nonuniformEXT(material.metallic_roughness_sampler_index)]), in_uv).xy;
    mtr *= vec2(material.normal_metallic_factor.w, material.emission_roughness_factor.w);
    float metallic = clamp(mtr.x, 0.0001, 0.9999);
    float roughness = clamp(mtr.y * mtr.y, 0.0001, 0.9999); // Perceptual roughness squared

    // Ambient occlusion
    float ao = texture(sampler2D(textures_2d[nonuniformEXT(material.occlusion_texture_index)], samplers[nonuniformEXT(material.occlusion_sampler_index)]), in_uv).r;
    ao = mix(1.0, ao, material.alpha_cutoff_occlusion_strength_reserved.y);

    // Normal mapping
    vec3 normal_sample = texture(sampler2D(textures_2d[nonuniformEXT(material.normal_texture_index)], samplers[nonuniformEXT(material.normal_sampler_index)]), in_uv).xyz;
    vec3 normal_tangent = (normal_sample * 2.0 - 1.0) * material.normal_metallic_factor.xyz;
    mat3 tbn = mat3(in_tng, in_btg, in_nrm);
    vec3 nrm = normalize(tbn * normal_tangent);

    // Emission
    vec3 ems = texture(sampler2D(textures_2d[nonuniformEXT(material.emission_texture_index)], samplers[nonuniformEXT(material.emission_sampler_index)]), in_uv).rgb;
    ems *= material.emission_roughness_factor.xyz;

    // Calculate F0 (surface reflection at zero incidence)
    // Dielectrics use 0.04, metals use albedo color
    vec3 f0 = mix(vec3(0.04), alb.rgb, metallic);

    // F90 for energy conservation (grazing angle reflectance)
    float f90 = clamp(dot(f0, vec3(50.0 * 0.33)), 0.0001, 1.0);

    // View direction (from surface to camera)
    vec3 eye = normalize(in_pos - camera.position_far.xyz);
    vec3 view = -eye;
    vec3 reflection = reflect(eye, nrm);

    float normal_dot_view = max(dot(nrm, view), 0.0001);
    vec3 fresnel = fresnel_schlick(normal_dot_view, f0, f90);

    vec3 illumination = vec3(0.0001);

    // ========== Point Lights ==========
    for (uint i = model.point_light_begin_index; i < model.point_light_end_index; ++i) {
        GxShaderDataPointLight light = point_lights[i];

        vec3 light_vec = light.position.xyz - in_pos;
        float distance_sq = dot(light_vec, light_vec);
        float distance = sqrt(distance_sq);
        vec3 light_dir = light_vec / distance;

        // Inverse square falloff with range cutoff
        float attenuation = 1.0 / max(distance_sq, 0.0001);

        illumination += compute_light(
            light_dir,
            light.colour.rgb * attenuation,
            view, nrm, normal_dot_view, roughness, metallic, f0, f90, alb.rgb);
    }

    // ========== Directional Lights (Non-Shadow Casting) ==========
    for (uint i = model.directional_light_begin_index; i < model.directional_light_end_index; ++i) {
        GxShaderDataDirectionalLight light = directional_lights[i];

        illumination += compute_light(
            light.direction.xyz,
            light.colour.rgb,
            view, nrm, normal_dot_view, roughness, metallic, f0, f90, alb.rgb);
    }

    // ========== Shadow-Casting Directional Lights ==========
    for (uint i = 0; i < model.shadow_caster_directional_lights_count; ++i) {
        GxShaderDataShadowCasterDirectionalLight light = shadow_caster_directional_lights[model.shadow_caster_directional_lights[i]];

        uint shadow_map_texture_index = floatBitsToUint(light.direction_bit_shadow_map_texture_index.w);

        // Shadow calculation
        float normal_dot_light = max(dot(nrm, light.direction_bit_shadow_map_texture_index.xyz), 0.00001);
        float shadow_bias = clamp(sqrt((0.000025 / (normal_dot_light * normal_dot_light)) - 0.000025), 0.001, 0.02);

        vec4 light_uv_depth = light.normalised_vp * vec4(in_pos, 1.0);
        light_uv_depth.xyz /= light_uv_depth.w;
        light_uv_depth.xy = light_uv_depth.xy * 0.5 + 0.5;
        light_uv_depth.z -= shadow_bias;

        // Check bounds
        vec3 uv_bounds = step(vec3(0.0), light_uv_depth.xyz) * step(light_uv_depth.xyz, vec3(1.0));
        float in_shadow_map = uv_bounds.x * uv_bounds.y * uv_bounds.z;

        // Sample shadow map using comparison sampler
        float shadow_sample = texture(sampler2DShadow(textures_2d[nonuniformEXT(shadow_map_texture_index)], shadow_sampler_cmp), light_uv_depth.xyz);

        float shadow_w = in_shadow_map * (1.0 - shadow_sample);
        float light_w = 1.0 - shadow_w;

        illumination += light_w * compute_light(
            light.direction_bit_shadow_map_texture_index.xyz,
            light.colour.rgb,
            view, nrm, normal_dot_view, roughness, metallic, f0, f90, alb.rgb);
    }

    // ========== Image-Based Lighting (IBL) ==========
    GxShaderDataReflectionProbe probe = reflection_probes[model.reflection_probe_index];

    // Diffuse IBL - irradiance map
    vec3 irr = texture(
        samplerCube(textures_cube[nonuniformEXT(probe.irradiance_texture_index)], samplers[nonuniformEXT(probe.irradiance_sampler_index)]),
        nrm).rgb;

    // Specular IBL - radiance (pre-filtered environment) map with LOD based on roughness
    float lod = mtr.y * probe.radiance_lod_coefficient;
    vec3 rad = textureLod(
        samplerCube(textures_cube[nonuniformEXT(probe.radiance_texture_index)], samplers[nonuniformEXT(probe.radiance_sampler_index)]),
        reflection, lod).rgb;

    // BRDF integration LUT
    vec2 brdf = texture(
        sampler2D(textures_2d[nonuniformEXT(scene.brdflut_texture_index)], samplers[nonuniformEXT(scene.brdflut_sampler_index)]),
        vec2(normal_dot_view, roughness)).rg;

    // Ambient diffuse (reduced by fresnel and metallic for energy conservation)
    vec3 ambient = irr * alb.rgb * (vec3(1.0) - fresnel) * (1.0 - metallic);

    // Ambient specular (using split-sum approximation)
    ambient += rad * ((f0 * brdf.x) + brdf.y);

    // Apply ambient occlusion
    ambient *= ao;

    // Scene ambient light (flat ambient term)
    ambient += scene.ambient_light.rgb * alb.rgb * ao;

    // Final color
    vec3 frag_colour = ambient + ems + illumination;

    frag_out = vec4(frag_colour, 1.0);
}