#ifndef GEAROENIX_RENDER_PIPELINE_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_RESOURCE_SET_HPP

#include <memory>

namespace gearoenix {
namespace render {
    namespace buffer {
        class Uniform;
    }
    namespace camera {
        class Camera;
    }
    namespace command {
        class Buffer;
    }
    namespace graph {
        namespace node {
            class Node;
        }
    }
    namespace light {
        class Light;
    }
    namespace material {
        class Material;
    }
    namespace mesh {
        class Mesh;
    }
    namespace model {
        class Model;
    }
    namespace scene {
        class Scene;
    }
    namespace texture {
        class Texture2D;
        class Cube;
    }
    namespace pipeline {
        class ResourceSet {
        protected:
            std::shared_ptr<buffer::Uniform> scene_uniform_buffer = nullptr;
            std::shared_ptr<buffer::Uniform> camera_uniform_buffer = nullptr;
            std::shared_ptr<buffer::Uniform> light_uniform_buffer = nullptr;
            std::shared_ptr<buffer::Uniform> model_uniform_buffer = nullptr;
            std::shared_ptr<buffer::Uniform> material_uniform_buffer = nullptr;
            std::shared_ptr<buffer::Uniform> node_uniform_buffer = nullptr;

            const mesh::Mesh* msh = nullptr;

            std::shared_ptr<texture::Texture2D> color = nullptr;
            std::shared_ptr<texture::Texture2D> metallic_roughness = nullptr;
            std::shared_ptr<texture::Texture2D> normal = nullptr;
            std::shared_ptr<texture::Texture2D> emissive = nullptr;

        public:
            virtual ~ResourceSet() = default;
#define GX_HELPER(c, cc) virtual void set_##c(const c::cc* o) noexcept
            GX_HELPER(scene, Scene);
            GX_HELPER(camera, Camera);
            GX_HELPER(light, Light);
            GX_HELPER(model, Model);
            GX_HELPER(mesh, Mesh);
            GX_HELPER(material, Material);
#undef GX_HELPER
            void set_node_uniform_buffer(const std::shared_ptr<buffer::Uniform>& node_uniform_buffer);
            virtual void clean();
        };
    }
}
}

#endif
