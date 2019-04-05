#ifndef GEAROEMIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_RESOURCE_SET_HPP
#define GEAROEMIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_RESOURCE_SET_HPP
#include "rnd-pip-pipeline.hpp"
#include "rnd-pip-resource-set.hpp"
//#include "../../math/math-matrix.hpp"
//#include "../../math/math-vector.hpp"
//#include "../../core/sync/cr-sync-end-caller.hpp"
//#include <vector>
#include <memory>

namespace gearoenix {
namespace render {
    namespace buffer {
        class Uniform;
        class Manager;
    }
    namespace camera {
        class Camera;
    }
    namespace command {
        class Buffer;
    }
    namespace light {
        class Directional;
    }
    namespace model {
        class Model;
    }
	namespace texture {
		class Cube;
	}
    namespace pipeline {
        class ForwardPbrDirectionalShadowResourceSet : public ResourceSet {
		protected:
			std::shared_ptr<texture::Cube> diffuse_environment = nullptr;
			std::shared_ptr<texture::Cube> specular_environment = nullptr;
			std::shared_ptr<texture::Texture2D> ambient_occlusion = nullptr;
			std::shared_ptr<texture::Texture2D> shadow_mapper = nullptr;
			std::shared_ptr<texture::Texture2D> brdflut = nullptr;
		public:
			virtual void set_node_uniform_buffer(const std::shared_ptr<buffer::Uniform> &uniform_buffer);
			virtual void set_diffuse_environment(const std::shared_ptr<texture::Cube>& t);
			virtual void set_specular_environment(const std::shared_ptr<texture::Cube>& t);
			virtual void set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t);
			virtual void set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t);
			virtual void set_brdflut(const std::shared_ptr<texture::Texture2D>& t);
			virtual void clean();
        };
    }
}
}
#endif
