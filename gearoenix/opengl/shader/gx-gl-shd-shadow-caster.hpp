#ifndef GEAROENIX_GL_SHADER_SHADOW_CASTER_HPP
#define GEAROENIX_GL_SHADER_SHADOW_CASTER_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <array>
#include <optional>

namespace gearoenix::gl::shader {
struct ShadowCaster final : public Shader {
    GX_GET_VAL_PRV(sint, mvp, GX_GL_UNIFORM_FAILED);
    GX_GL_UNIFORM_VECTOR(alpha_factor_alpha_cutoff, 2, 1);
    GX_GL_UNIFORM_TEXTURE(albedo);

public:
    const sizei mvp_count;

    explicit ShadowCaster(Engine& e, std::size_t bones_count = 0);
    ~ShadowCaster() final;
    void bind(uint& current_shader) const final;
    void set_mvp_data(const void* data) const;
};

struct ShadowCasterCombination final : public ShaderCombination {
    friend struct Manager;

    Engine& e;

private:
    std::array<std::optional<ShadowCaster>, GX_RENDER_MAX_BONES_COUNT + 1> shaders;

    explicit ShadowCasterCombination(Engine& e);

public:
    ~ShadowCasterCombination() final = default;
    [[nodiscard]] const ShadowCaster& get(std::size_t bones_count = 0)
    {
        auto& s = shaders[bones_count];
        if (s.has_value())
            return s.value();
        return s.emplace(e, bones_count);
    }
};
}

#endif
#endif