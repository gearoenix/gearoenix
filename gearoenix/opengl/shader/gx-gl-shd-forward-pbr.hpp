#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <array>
#include <boost/container/static_vector.hpp>
#include <optional>

namespace gearoenix::gl::shader {
struct ForwardPbr final : Shader {
    friend struct ForwardPbrCombination;

    // Camera data ---------------------------------------
    GX_GL_UNIFORM_MATRIX(vp, 4, 1);
    GX_GL_UNIFORM_VECTOR(camera_position_reserved, 4, 1);
    // Model data ----------------------------------------
    GX_GL_UNIFORM_MATRIX(m, 4, 1);
    GX_GL_UNIFORM_MATRIX(inv_m, 4, 1);
    GX_GL_UNIFORM_VECTOR(albedo_factor, 4, 1);
    GX_GL_UNIFORM_VECTOR(normal_metallic_factor, 4, 1);
    GX_GL_UNIFORM_VECTOR(emission_roughness_factor, 4, 1);
    GX_GL_UNIFORM_VECTOR(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved, 4, 1);
    // Textures ------------------------------------------
    GX_GL_UNIFORM_TEXTURE(albedo);
    GX_GL_UNIFORM_TEXTURE(normal);
    GX_GL_UNIFORM_TEXTURE(emission);
    GX_GL_UNIFORM_TEXTURE(metallic_roughness);
    GX_GL_UNIFORM_TEXTURE(occlusion);
    GX_GL_UNIFORM_TEXTURE(irradiance);
    GX_GL_UNIFORM_TEXTURE(radiance);
    GX_GL_UNIFORM_TEXTURE(brdflut);

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
        std::uint32_t directional_lights_count,
        std::uint32_t shadow_casters_directional_lights_count,
        std::uint32_t bones_count);
    ForwardPbr(ForwardPbr&&) noexcept;
    ~ForwardPbr() override;
    void bind(uint& current_shader) const override;
    void set_shadow_caster_directional_light_normalised_vp_data(const void* data);
    void set_shadow_caster_directional_light_direction_data(const void* data);
    void set_shadow_caster_directional_light_colour_data(const void* data);
    [[nodiscard]] const sint* get_shadow_caster_directional_light_shadow_map_indices() const;

    void set_bones_m_inv_m_data(const void* data);

    void set_directional_light_direction_data(const void* data);
    void set_directional_light_colour_data(const void* data);
};

struct ForwardPbrCombination final : ShaderCombination {
    friend struct Manager;

private:
    typedef std::array<std::optional<ForwardPbr>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS + 1> directional_lights;
    typedef std::array<directional_lights, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER + 1> shadow_caster_directional_lights;
    typedef std::array<shadow_caster_directional_lights, GX_RENDER_MAX_BONES_COUNT + 1> bones;

    bones combinations;

    ForwardPbrCombination()
    {
    }

public:
    [[nodiscard]] ForwardPbr& get(const std::uint32_t bones_count, const std::uint32_t shadow_casters_directional_lights_count, const std::uint32_t directional_lights_count)
    {
        auto& s = combinations[bones_count][shadow_casters_directional_lights_count][directional_lights_count];
        if (s.has_value()) {
            return s.value();
        }
        s.emplace(directional_lights_count, shadow_casters_directional_lights_count, bones_count);
        return s.value();
    }
};
}

#endif