#ifndef GEAROENIX_RENDER_BUFFER_MESH_HPP
#define GEAROENIX_RENDER_BUFFER_MESH_HPP
#include <memory>
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    class Engine;
    namespace buffer {
        class Mesh {
        protected:
            Engine* engine;

        public:
            Mesh(Engine* engine);
            virtual ~Mesh();
            virtual void bind() = 0;
            virtual void draw() = 0;
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_MESH_HPP
