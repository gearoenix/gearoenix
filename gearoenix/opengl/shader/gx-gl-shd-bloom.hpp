#ifndef GEAROENIX_GL_SHADER_BLOOM_HPP
#define GEAROENIX_GL_SHADER_BLOOM_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <boost/container/static_vector.hpp>

namespace gearoenix::gl::shader {
struct Bloom final : public Shader {
    GX_GL_UNIFORM_VECTOR(screen_space_uv, 2, 1)
    GX_GL_UNIFORM_TEXTURE(source_texture)

    Bloom(Engine& e, bool is_horizontal) noexcept;
    ~Bloom() noexcept final;
    void bind() const noexcept final;
};

struct BoolHorizontalCombination final {
private:
    boost::container::static_vector<Bloom, 2> bloom_combination;

public:
    explicit BoolHorizontalCombination(Engine& e) noexcept;
    [[nodiscard]] Bloom& get_shader(bool is_horizontal) noexcept;
};

typedef BoolHorizontalCombination BloomCombination;
}

#endif
#endif