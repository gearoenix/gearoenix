#ifndef GEAROENIX_GLES2_BUFFER_MESH_HPP
#define GEAROENIX_GLES2_BUFFER_MESH_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/buffer/rnd-buf-mesh.hpp"
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace buffer {
        class Mesh : public render::buffer::Mesh {
        public:
        private:
        public:
            Mesh(unsigned int vertex_elements_count, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~Mesh();
            void bind();
            void draw();
        };
    } // namespace buffer
} // namespace gles2
} // namespace gearoenix
#endif
#endif // GEAROENIX_GLES2_BUFFER_MESH_HPP
