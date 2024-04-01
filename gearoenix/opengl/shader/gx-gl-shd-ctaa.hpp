#ifndef GEAROENIX_GL_SHADER_COLOUR_TUNING_ANTI_ALIASING_HPP
#define GEAROENIX_GL_SHADER_COLOUR_TUNING_ANTI_ALIASING_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/camera/gx-rnd-cmr-colour-tuning.hpp"

namespace gearoenix::gl::shader {
struct ColourTuningAntiAliasing final : public Shader {
    GX_GL_UNIFORM_VECTOR(screen_space_uv, 2, 1);
    GX_GL_UNIFORM_VECTOR(gamma_exponent, 3, 1);
    GX_GL_UNIFORM_VECTOR(colour_scale, 3, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);
    GX_GL_UNIFORM_TEXTURE(depth_texture);

public:
    ColourTuningAntiAliasing(Engine& e, std::size_t colour_tuning_index);
    ~ColourTuningAntiAliasing() final;
    void bind(uint& current_shader) const final;
    void set(const render::camera::ColourTuning& colour_tuning);
};

struct ColourTuningAntiAliasingCombination final : public ShaderCombination {
    friend struct Manager;

    Engine& e;

private:
    std::array<std::optional<ColourTuningAntiAliasing>, std::variant_size_v<render::camera::ColourTuning>> combination;

    explicit ColourTuningAntiAliasingCombination(Engine& e);

public:
    [[nodiscard]] ColourTuningAntiAliasing& get(const render::camera::ColourTuning& colour_tuning)
    {
        auto& result = combination[colour_tuning.index()];
        if (result.has_value())
            return result.value();
        result.emplace(e, colour_tuning.index());
        return result.value();
    }
};
}

#endif
#endif