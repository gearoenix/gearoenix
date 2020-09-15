#ifndef GEAROENIX_GLES2_BUFFER_VERTEX_HPP
#define GEAROENIX_GLES2_BUFFER_VERTEX_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "../../render/buffer/gx-rnd-buf-static.hpp"
#include <vector>

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::buffer {
class Vertex final : public render::buffer::Static {
public:
private:
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
