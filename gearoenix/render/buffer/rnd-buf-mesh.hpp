#ifndef GEAROENIX_RENDER_BUFFER_MESH_HPP
#define GEAROENIX_RENDER_BUFFER_MESH_HPP
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
        class Mesh {
        public:
        private:
        public:
            virtual ~Mesh();
            virtual void bind() = 0;
            virtual void draw() = 0;
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_MESH_HPP
