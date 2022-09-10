#ifndef GEAROENIX_GL_SHADER_FORWARD_PBR_HPP
#define GEAROENIX_GL_SHADER_FORWARD_PBR_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <boost/container/static_vector.hpp>

namespace gearoenix::gl::shader {
struct ForwardPbr final : public Shader {
    // Camera data ---------------------------------------
    GX_GL_UNIFORM_MATRIX(vp, 4, 1)
    GX_GL_UNIFORM_VECTOR(camera_position_reserved, 4, 1)
    // Model data ----------------------------------------
    GX_GL_UNIFORM_MATRIX(m, 4, 1)
    GX_GL_UNIFORM_MATRIX(inv_m, 4, 1)
    GX_GL_UNIFORM_VECTOR(albedo_factor, 4, 1)
    GX_GL_UNIFORM_VECTOR(normal_metallic_factor, 4, 1)
    GX_GL_UNIFORM_VECTOR(emission_roughness_factor, 4, 1)
    GX_GL_UNIFORM_VECTOR(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved, 4, 1)
    // Textures ------------------------------------------
    GX_GL_UNIFORM_TEXTURE(albedo)
    GX_GL_UNIFORM_TEXTURE(normal)
    GX_GL_UNIFORM_TEXTURE(emission)
    GX_GL_UNIFORM_TEXTURE(metallic_roughness)
    GX_GL_UNIFORM_TEXTURE(occlusion)
    GX_GL_UNIFORM_TEXTURE(irradiance)
    GX_GL_UNIFORM_TEXTURE(radiance)
    GX_GL_UNIFORM_TEXTURE(brdflut)

private:
    sint directional_light_direction = GX_GL_UNIFORM_FAILED;
    sint directional_light_colour = GX_GL_UNIFORM_FAILED;
    sizei directional_lights_count = static_cast<sizei>(-1);

    sint shadow_caster_directional_light_normalised_vp = GX_GL_UNIFORM_FAILED;
    sint shadow_caster_directional_light_direction = GX_GL_UNIFORM_FAILED;
    sint shadow_caster_directional_light_colour = GX_GL_UNIFORM_FAILED;
    sint shadow_caster_directional_light_shadow_map = GX_GL_UNIFORM_FAILED;
    boost::container::static_vector<sint, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_light_shadow_map_indices;

    sint bones_m_inv_m = GX_GL_UNIFORM_FAILED;
    sizei bones_matrices_count = static_cast<sizei>(-1);

public:
    ForwardPbr(
        Engine& e,
        std::size_t directional_lights_count,
        std::size_t shadow_casters_directional_lights_count,
        std::size_t bones_count) noexcept;
    ForwardPbr(ForwardPbr&&) noexcept;
    ~ForwardPbr() noexcept final;
    void bind() const noexcept final;
    void set_shadow_caster_directional_light_normalised_vp_data(const void* data) noexcept;
    void set_shadow_caster_directional_light_direction_data(const void* data) noexcept;
    void set_shadow_caster_directional_light_colour_data(const void* data) noexcept;
    [[nodiscard]] const sint* get_shadow_caster_directional_light_shadow_map_indices() const noexcept;

    void set_bones_m_inv_m_data(const void* data) noexcept;

    void set_directional_light_direction_data(const void* data) noexcept;
    void set_directional_light_colour_data(const void* data) noexcept;
};

struct ForwardPbrDirectionalLightCountCombination final {
private:
    boost::container::static_vector<ForwardPbr, GX_RENDER_MAX_DIRECTIONAL_LIGHTS + 1> shaders;

public:
    ForwardPbrDirectionalLightCountCombination(Engine& e, std::size_t shadow_caster_directional_lights_count, std::size_t bones_count) noexcept;
    [[nodiscard]] ForwardPbr* get_shader_for_directional_lights_count(std::size_t c) noexcept;
};

struct ForwardPbrShadowCastersDirectionalLightCountCombination final {
private:
    boost::container::static_vector<ForwardPbrDirectionalLightCountCombination, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER + 1> directional_lights_count_combination;

public:
    ForwardPbrShadowCastersDirectionalLightCountCombination(Engine& e, std::size_t bones_count) noexcept;
    [[nodiscard]] ForwardPbrDirectionalLightCountCombination& get_shader_for_shadow_caster_directional_lights_count(std::size_t c) noexcept;
};

struct ForwardPbrBonesCountCombination final {
private:
    boost::container::static_vector<ForwardPbrShadowCastersDirectionalLightCountCombination, GX_RENDER_MAX_BONES_COUNT + 1> shadow_caster_directional_lights_count_combination;

public:
    explicit ForwardPbrBonesCountCombination(Engine& e) noexcept;
    [[nodiscard]] ForwardPbrShadowCastersDirectionalLightCountCombination& get_shader_for_bones_count_combination(std::size_t c) noexcept;
};

typedef ForwardPbrBonesCountCombination ForwardPbrCombination;
}

#endif
#endif