#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <array>
#include <optional>

namespace gearoenix::gl::shader {
struct ShadowCaster final : Shader {
    GX_GET_VAL_PRV(sint, mvp, GX_GL_UNIFORM_FAILED);
    GX_GL_UNIFORM_VECTOR(alpha_factor_alpha_cutoff, 2, 1);
    GX_GL_UNIFORM_TEXTURE(albedo);

public:
    const sizei mvp_count;

    explicit ShadowCaster(std::uint32_t bones_count = 0);
    ~ShadowCaster() override;
    void bind(uint& current_shader) const override;
    void set_mvp_data(const float* data) const;
};

struct ShadowCasterCombination final : ShaderCombination {
    friend struct Manager;

private:
    std::array<std::optional<ShadowCaster>, GX_RENDER_MAX_BONES_COUNT + 1> shaders;

    ShadowCasterCombination();

public:
    ~ShadowCasterCombination() override = default;
    [[nodiscard]] const ShadowCaster& get(std::uint32_t bones_count = 0)
    {
        auto& s = shaders[bones_count];
        if (s.has_value()) {
            return s.value();
        }
        return s.emplace(bones_count);
    }
};
}

#endif