#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/camera/gx-rnd-cmr-colour-tuning.hpp"

namespace gearoenix::gl::shader {
struct ColourTuningAntiAliasing final : Shader {
    GX_GL_UNIFORM_VECTOR(screen_space_uv, 2, 1);
    GX_GL_UNIFORM_VECTOR(gamma_exponent, 3, 1);
    GX_GL_UNIFORM_VECTOR(colour_scale, 3, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);
    GX_GL_UNIFORM_TEXTURE(depth_texture);

public:
    explicit ColourTuningAntiAliasing(std::uint32_t colour_tuning_index);
    ~ColourTuningAntiAliasing() override;
    void bind(uint& current_shader) const override;
    void set(const render::camera::ColourTuning& colour_tuning);
};

struct ColourTuningAntiAliasingCombination final : ShaderCombination {
    friend struct Manager;

private:
    std::array<std::optional<ColourTuningAntiAliasing>, render::camera::ColourTuning::types_count> combination;

    ColourTuningAntiAliasingCombination();

public:
    [[nodiscard]] ColourTuningAntiAliasing& get(const render::camera::ColourTuning& colour_tuning)
    {
        auto& result = combination[colour_tuning.get_index()];
        if (result.has_value()) {
            return result.value();
        }
        result.emplace(static_cast<std::uint32_t>(colour_tuning.get_index()));
        return result.value();
    }
};
}

#endif