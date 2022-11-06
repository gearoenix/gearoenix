#ifndef GEAROENIX_GL_SHADER_BLOOM_HPP
#define GEAROENIX_GL_SHADER_BLOOM_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <array>
#include <optional>

namespace gearoenix::gl::shader {
struct Bloom final : public Shader {
    GX_GL_UNIFORM_VECTOR(screen_space_uv, 2, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    Bloom(Engine& e, bool is_horizontal) noexcept;
    ~Bloom() noexcept final;
    void bind(uint& current_shader) const noexcept final;
};

struct BloomCombination final : public ShaderCombination {
    friend struct Manager;

    Engine& e;

private:
    std::array<std::optional<Bloom>, 2> bloom_combination;

    explicit BloomCombination(Engine& e) noexcept;

public:
    [[nodiscard]] Bloom& get(const bool is_horizontal) noexcept
    {
        auto& result = bloom_combination[is_horizontal ? 1 : 0];
        if (result.has_value())
            return result.value();
        result.emplace(e, is_horizontal);
        return result.value();
    }
};

}

#endif
#endif