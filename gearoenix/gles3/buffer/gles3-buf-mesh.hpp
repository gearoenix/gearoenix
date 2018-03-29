#ifndef GEAROENIX_gles3_BUFFER_MESH_HPP
#define GEAROENIX_gles3_BUFFER_MESH_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../render/buffer/rnd-buf-mesh.hpp"
#include "../gles3.hpp"
namespace gearoenix {
namespace gles3 {
    class Engine;
    namespace buffer {
        class Mesh : public render::buffer::Mesh {
        public:
        private:
            GLuint vao, vbo, ibo, ic;

        public:
            Mesh(unsigned int vertex_elements_count, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            ~Mesh();
            void bind();
            void draw();
        };
    } // namespace buffer
} // namespace gles3
} // namespace gearoenix
#endif
#endif // GEAROENIX_gles3_BUFFER_MESH_HPP
