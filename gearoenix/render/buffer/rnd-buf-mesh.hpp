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
    namespace engine {
        class Engine;
    }
    namespace buffer {
        class Mesh {
        protected:
            const std::shared_ptr<engine::Engine> e;

        public:
            Mesh(const std::shared_ptr<engine::Engine>& e);
            virtual ~Mesh();
            virtual void bind() = 0;
            virtual void draw() = 0;
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_MESH_HPP
