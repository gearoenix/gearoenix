#ifndef GEAROEMIX_RENDER_PIPELINE_RESOURCE_SET_HPP
#define GEAROEMIX_RENDER_PIPELINE_RESOURCE_SET_HPP

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
			
			std::shared_ptr<mesh::Mesh> msh = nullptr;

			std::shared_ptr<texture::Texture2D> color = nullptr;
			std::shared_ptr<texture::Texture2D> metallic_roughness = nullptr;
			std::shared_ptr<texture::Texture2D> normal = nullptr;
			std::shared_ptr<texture::Texture2D> emissive = nullptr;
        public:
            virtual ~ResourceSet();
#define GXHELPER(c, cc) virtual void set_##c(const std::shared_ptr<c::cc>& o)
			GXHELPER(scene, Scene);
            GXHELPER(camera, Camera);
            GXHELPER(light, Light);
            GXHELPER(model, Model);
            GXHELPER(mesh, Mesh);
            GXHELPER(material, Material);
#undef GXHELPER
            virtual void clean();
        };
    }
}
}

#endif
