#ifndef GEAROENIX_GL_SHADER_SHADOW_CASTER_HPP
#define GEAROENIX_GL_SHADER_SHADOW_CASTER_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <boost/container/static_vector.hpp>

namespace gearoenix::gl::shader {
struct ShadowCaster final : public Shader {
    GX_GET_VAL_PRV(sint, mvp, GX_GL_UNIFORM_FAILED);
    GX_GL_UNIFORM_VECTOR(alpha_factor_alpha_cutoff, 2, 1);
    GX_GL_UNIFORM_TEXTURE(albedo);

public:
    const sizei mvp_count;

    explicit ShadowCaster(Engine& e, std::size_t bones_count = 0) noexcept;
    ~ShadowCaster() noexcept final;
    void bind() const noexcept final;
    void set_mvp_data(const void* data) const noexcept;
};

struct ShadowCasterBoneCountCombination final {
private:
    boost::container::static_vector<ShadowCaster, 64> shaders;

public:
    explicit ShadowCasterBoneCountCombination(Engine& e) noexcept;
    [[nodiscard]] const ShadowCaster& get_shader(std::size_t bones_count = 0) const noexcept;
};

typedef ShadowCasterBoneCountCombination ShadowCasterCombination;
}

#endif
#endif