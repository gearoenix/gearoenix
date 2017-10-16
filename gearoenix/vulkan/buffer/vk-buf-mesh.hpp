#ifndef GEAROENIX_VULKAN_BUFFER_MESH_HPP
#define GEAROENIX_VULKAN_BUFFER_MESH_HPP
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
            SubBuffer* vrtbuf;
            SubBuffer* indbuf;
            unsigned int vc; // I have doubt, it is necessary to keep this I will see it near future
            unsigned int vs; // I have doubt, it is necessary to keep this I will see it near future
            unsigned int ic;
            unsigned int is; // I have doubt, it is necessary to keep this I will see it near future

        public:
            Mesh(unsigned int vertex_elements_count, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~Mesh();
            void bind();
            void draw();
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_BUFFER_MESH_HPP
