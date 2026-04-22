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

// Roughness-damped Fresnel for IBL (avoids overly bright edges on rough surfaces)
vec3 fresnel_schlick_roughness(const float cos_theta, const vec3 f0, const float roughness) {
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
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
    const GxShaderDataScene scene = scenes[pc.scene_index];
    const GxShaderDataCamera camera = cameras[pc.camera_index];
    const GxShaderDataModel model = models[pc.model_index];
    const GxShaderDataMaterial material = materials[pc.material_index];

    // Albedo
    const vec4 alb = sample_texture_2d(material.albedo_normal_texture_sampler_index.xy, in_uv) * material.albedo_factor;

    // Alpha test
    if (alb.a <= material.alpha_cutoff_occlusion_strength_reserved_reserved.x) {
        discard;
    }

    const vec3 orm_sample = sample_texture_2d(material.orm_emission_texture_sampler_index.xy, in_uv).xyz;
    const float metallic = clamp(orm_sample.z * material.normal_metallic_factor.w, 0.0001, 0.9999);
    const float sqrt_roughness = orm_sample.y * material.emission_roughness_factor.w;
    const float roughness = clamp(sqrt_roughness * sqrt_roughness, 0.0001, 0.9999); // Perceptual roughness squared

    // Ambient occlusion
    const float ao = mix(1.0, orm_sample.x, material.alpha_cutoff_occlusion_strength_reserved_reserved.y);

    // Normal mapping
    const vec3 normal_sample = sample_texture_2d(material.albedo_normal_texture_sampler_index.zw, in_uv).xyz;
    const vec3 normal_tangent = (normal_sample * 2.0 - 1.0) * material.normal_metallic_factor.xyz;
    const mat3 tbn = mat3(in_tng, in_btg, in_nrm);
    const vec3 nrm = normalize(tbn * normal_tangent);

    // Emission
    vec3 ems = sample_texture_2d(material.orm_emission_texture_sampler_index.zw, in_uv).rgb;
    ems *= material.emission_roughness_factor.xyz;

    // Calculate F0 (surface reflection at zero incidence)
    // Dielectrics use 0.04, metals use albedo color
    const vec3 f0 = mix(vec3(0.04), alb.rgb, metallic);

    // F90 for energy conservation (grazing angle reflectance)
    const float f90 = clamp(dot(f0, vec3(50.0 * 0.33)), 0.0001, 1.0);

    // View direction (from surface to camera)
    const vec3 eye = normalize(in_pos - camera.position_far.xyz);
    const vec3 view = -eye;
    const vec3 reflection = reflect(eye, nrm);

    const float normal_dot_view = max(dot(nrm, view), 0.0001);
    const vec3 fresnel = fresnel_schlick_roughness(normal_dot_view, f0, roughness);

    vec3 illumination = vec3(0.0001);

    // uvec4 layout: x=bones, y=point_lights, z=directional_lights, w=shadow_caster_directional_lights
    const uvec4 lights_counts = model.bones_point_lights_directional_lights_shadow_caster_directional_lights_count;

    // ========== Point Lights ==========
    for (uint i = 0; i < lights_counts.y; ++i) {
        const uint light_index = model.point_lights[i >> 2u][i & 3u];
        GxShaderDataPointLight light = point_lights[light_index];

        vec3 light_vec = light.position.xyz - in_pos;
        float distance_sq = dot(light_vec, light_vec);
        float inv_dist = inversesqrt(max(distance_sq, 0.0001));
        vec3 light_dir = light_vec * inv_dist;

        // Inverse square falloff with range cutoff
        float attenuation = inv_dist * inv_dist;

        illumination += compute_light(
            light_dir,
            light.colour.rgb * attenuation,
            view, nrm, normal_dot_view, roughness, metallic, f0, f90, alb.rgb);
    }

    // ========== Directional Lights (Non-Shadow Casting) ==========
    for (uint i = 0; i < lights_counts.z; ++i) {
        const uint light_index = model.directional_lights[i >> 2u][i & 3u];
        GxShaderDataDirectionalLight light = directional_lights[light_index];

        illumination += compute_light(
            light.direction.xyz,
            light.colour.rgb,
            view, nrm, normal_dot_view, roughness, metallic, f0, f90, alb.rgb);
    }

    // ========== Shadow-Casting Directional Lights ==========
    for (uint i = 0; i < lights_counts.w; ++i) {
        const uint light_index = model.shadow_caster_directional_lights[i >> 2u][i & 3u];
        const GxShaderDataShadowCasterDirectionalLight light = shadow_caster_directional_lights[light_index];

        const uint shadow_map_texture_index = floatBitsToUint(light.direction_bit_shadow_map_texture_index.w);

        // Shadow calculation
        const float normal_dot_light = max(dot(nrm, light.direction_bit_shadow_map_texture_index.xyz), 0.00001);
        const float shadow_bias = clamp(sqrt((0.000025 / (normal_dot_light * normal_dot_light)) - 0.000025), 0.001, 0.02);

        vec4 light_uv_depth = light.normalised_vp * vec4(in_pos, 1.0);
        light_uv_depth.xyz /= light_uv_depth.w;
        light_uv_depth.xy = light_uv_depth.xy * 0.5 + 0.5;
        light_uv_depth.z -= shadow_bias;

        // Check bounds
        const vec3 uv_bounds = step(vec3(0.0), light_uv_depth.xyz) * step(light_uv_depth.xyz, vec3(1.0));
        const float in_shadow_map = uv_bounds.x * uv_bounds.y * uv_bounds.z;

        // Sample shadow map using comparison sampler (uses separate binding for Metal depth2d compatibility)
        const float shadow_sample = sample_shadow_2d_cmp(shadow_map_texture_index, light_uv_depth.xyz);

        const float shadow_w = in_shadow_map * (1.0 - shadow_sample);
        const float light_w = 1.0 - shadow_w;

        if (light_w > 0.0001) {
            illumination += light_w * compute_light(
                light.direction_bit_shadow_map_texture_index.xyz, light.colour.rgb, view, nrm, normal_dot_view,
                roughness, metallic, f0, f90, alb.rgb);
        }
    }

    // ========== Image-Based Lighting (IBL) ==========
    const GxShaderDataReflectionProbe probe = reflection_probes[model.reflection_probe_bones_begin_reserved_reserved.x];

    // Diffuse IBL - irradiance map (texture/sampler in .xy)
    const vec3 irr = sample_texture_cube(probe.irradiance_radiance_texture_sampler_index.xy, nrm).rgb;

    // Specular IBL - radiance (pre-filtered environment) map with LOD based on roughness (texture/sampler in .zw)
    const float rad_lod = sqrt_roughness * probe.radiance_lod_coefficient.x;
    const vec3 rad = sample_texture_cube_lod(probe.irradiance_radiance_texture_sampler_index.zw, reflection, rad_lod).rgb;

    // BRDF integration LUT
    const vec2 brdf = sample_texture_2d(scene.brdflut_sheen_lut_texture_sampler_index.xy, vec2(normal_dot_view, roughness)).rg;

    // Ambient diffuse (reduced by fresnel and metallic for energy conservation)
    vec3 ambient = irr * alb.rgb * (vec3(1.0) - fresnel) * (1.0 - metallic);

    // Ambient specular (using split-sum approximation)
    ambient += rad * ((f0 * brdf.x) + brdf.y);

    // Apply ambient occlusion
    ambient *= ao;

    // Scene ambient light (flat ambient term)
    ambient += scene.ambient_light_layer.rgb * alb.rgb * ao;

    // Final color
    const vec3 frag_colour = ambient + ems + illumination;

    frag_out = vec4(frag_colour, 1.0);
}