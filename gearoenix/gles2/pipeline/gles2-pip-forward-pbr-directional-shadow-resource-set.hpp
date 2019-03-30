#ifndef GEAROEMIX_GLES2_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_RESOURCE_SET_HPP
#define GEAROEMIX_GLES2_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_RESOURCE_SET_HPP
#include "gles2-pip-resource-set.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
namespace gearoenix {
	namespace gles2 {
		namespace shader {
			class ForwardPbrDirectionalShadow;
		}
    namespace pipeline {
        class ForwardPbrDirectionalShadowResourceSet : public ResourceSet {
		private:
			std::shared_ptr<>
		public:
			ForwardPbrDirectionalShadowResourceSet(const std::shared_ptr<shader::ForwardPbrDirectionalShadow> &shd);
			void set_scene(const std::shared_ptr<scene::Scene>& s) = 0;
			void set_camera(const std::shared_ptr<camera::Camera>& c) = 0;
			void set_light(const std::shared_ptr<light::Light>& l) = 0;
			void set_model(const std::shared_ptr<model::Model>& m) = 0;
			void set_mesh(const std::shared_ptr<mesh::Mesh>& m) = 0;
			void set_material(const std::shared_ptr<material::Material>& m) = 0;
			void set_node(const graph::node::Node *const node) = 0;
        };
    }
}
}
#endif
