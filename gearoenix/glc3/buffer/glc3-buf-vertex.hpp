#ifndef GEAROENIX_GLC3_BUFFER_VERTEX_HPP
#define GEAROENIX_GLC3_BUFFER_VERTEX_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../math/math-vertex.hpp"
#include "../../render/buffer/rnd-buf-static.hpp"
#include <vector>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::buffer {
class Vertex final : public render::buffer::Static {
public:
private:
    gl::uint vo = 0;
    gl::uint bo = 0;

public:
    Vertex(
        const std::vector<math::BasicVertex>& vertices,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Vertex() noexcept final;
    void bind() const noexcept;
};
}

#endif
#endif
