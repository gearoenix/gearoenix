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
    sint shadow_caster_directional_light_normalised_vp = GX_GL_UNIFORM_FAILED;
    sint shadow_caster_directional_light_direction = GX_GL_UNIFORM_FAILED;
    sint shadow_caster_directional_light_colour = GX_GL_UNIFORM_FAILED;
    sint shadow_caster_directional_light_shadow_map = GX_GL_UNIFORM_FAILED;
    boost::container::static_vector<sint, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_light_shadow_map_indices;

public:
    ForwardPbr(Engine& e, std::size_t shadow_casters_directional_lights_count) noexcept;
    ~ForwardPbr() noexcept final;
    void bind() const noexcept final;
    void set_shadow_caster_directional_light_normalised_vp_data(const void* data) noexcept;
    void set_shadow_caster_directional_light_direction_data(const void* data) noexcept;
    void set_shadow_caster_directional_light_colour_data(const void* data) noexcept;
    [[nodiscard]] const sint* get_shadow_caster_directional_light_shadow_map_indices() const noexcept;
};
}

#endif
#endif