#ifndef GEAROENIX_GLES2_BUFFER_VERTEX_HPP
#define GEAROENIX_GLES2_BUFFER_VERTEX_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../math/math-vertex.hpp"
#include "../../render/buffer/rnd-buf-static.hpp"
#include "../gles2.hpp"
#include <vector>
namespace gearoenix {
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace buffer {
        class Vertex : public render::buffer::Static {
        public:
        private:
            gl::uint bo = 0;

        public:
            Vertex(
                const std::vector<math::BasicVertex> vertices,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            ~Vertex();
            void bind() const;
        };
    }
}
}
#endif
#endif
