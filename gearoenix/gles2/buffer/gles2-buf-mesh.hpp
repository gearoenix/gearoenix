#ifndef GEAROENIX_GLES2_BUFFER_MESH_HPP
#define GEAROENIX_GLES2_BUFFER_MESH_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../render/buffer/rnd-buf-mesh.hpp"
#include "../gles2.hpp"
namespace gearoenix {
namespace gles2 {
	namespace engine {
		class Engine;
	}
    namespace buffer {
        class Mesh : public render::buffer::Mesh {
        public:
        private:
            GLuint vbo, ibo;
            GLsizei ic;

        public:
            Mesh(
				const std::shared_ptr<system::stream::Stream> &f, 
				const std::shared_ptr<engine::Engine> &e, 
				const core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            ~Mesh();
            void bind();
            void draw();
        };
    } // namespace buffer
} // namespace gles2
} // namespace gearoenix
#endif
#endif // GEAROENIX_GLES2_BUFFER_MESH_HPP
