#ifndef GEAROENIX_GL_SHADER_UNLIT_HPP
#define GEAROENIX_GL_SHADER_UNLIT_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <array>
#include <optional>

namespace gearoenix::gl::shader {
struct Unlit final : public Shader {
    GX_GL_UNIFORM_MATRIX(mvp, 4, 1);
    GX_GL_UNIFORM_VECTOR(albedo_factor, 4, 1);
    GX_GL_UNIFORM_VECTOR(uv_transform, 4, 1);
    GX_GL_UNIFORM_FLOAT(alpha_cutoff, 1);
    GX_GL_UNIFORM_TEXTURE(albedo);

public:
    Unlit(Engine& e, bool has_uv_transform, bool has_alpha_cutoff, bool has_albedo_factor, bool has_albedo) noexcept;
    ~Unlit() noexcept final;
    void bind(uint& current_shader) const noexcept final;
};

struct UnlitCombination final : public ShaderCombination {
    friend struct Manager;
    Engine& e;

private:
    typedef std::array<std::optional<Unlit>, 2> has_albedo;
    typedef std::array<has_albedo, 2> has_albedo_factor;
    typedef std::array<has_albedo_factor, 2> has_alpha_cutoff;
    typedef std::array<has_alpha_cutoff, 2> has_uv_transform;

    has_uv_transform combinations;

    explicit UnlitCombination(Engine& e) noexcept
        : e(e)
    {
    }

public: /// TODO: support bone-count too
    [[nodiscard]] Unlit& get(const bool uv_transform, const bool alpha_cutoff, const bool albedo_factor, const bool albedo) noexcept
    {
        auto& s = combinations
            [static_cast<std::size_t>(uv_transform)]
            [static_cast<std::size_t>(alpha_cutoff)]
            [static_cast<std::size_t>(albedo_factor)]
            [static_cast<std::size_t>(albedo)];
        if (s.has_value())
            return s.value();
        s.emplace(e, uv_transform, alpha_cutoff, albedo_factor, albedo);
        return s.value();
    }
};
}

#endif
#endif