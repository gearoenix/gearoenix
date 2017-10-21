#ifndef GEAROENIX_OPENGL_BUFFER_MESH_HPP
#define GEAROENIX_OPENGL_BUFFER_MESH_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include <memory>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace buffer {
        class SubBuffer;
        class Mesh {
        public:
            typedef uint32_t IndexType;

        private:
        public:
            Mesh(unsigned int vertex_elements_count, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~Mesh();
            void bind();
            void draw();
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif
#endif // GEAROENIX_OPENGL_BUFFER_MESH_HPP
