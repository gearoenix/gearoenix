#ifndef GEAROEMIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#define GEAROEMIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#include "rnd-gr-nd-node.hpp"
#include "../../../math/math-matrix.hpp"
#include "../../../math/math-vector.hpp"
#include "../../../core/cr-build-configuration.hpp"
#include <vector>
#include <memory>

namespace gearoenix {
	namespace render {
		namespace buffer {
			class Uniform;
			class Manager;
		}
		namespace command {
			class Buffer;
		}
		namespace pipeline {
			class Resource;
			class ResourceSet;
		}
		namespace sync {
			class Semaphore;
		}
		namespace texture {
			class Texture;
		}
		namespace graph {
			namespace node {
				// This only renders one cascaded directional light
				class ForwardPbr : public Node {
				public:
					struct Uniform {
						math::Mat4x4 mvp = math::Mat4x4();
						math::Mat4x4 directional_light_view_projection_biases[GX_MAX_SHADOW_MAPS];
						math::Vec4 direction_light_color;
						math::Vec4 direction_light_direction_count;
					};
				private:
					struct Frame {
						struct Kernel {
							command::Buffer *secondary_cmd = nullptr;
							std::shared_ptr<buffer::Manager> buffer_manager = nullptr;
							unsigned int latest_render_data_pool = 0;
							std::vector<std::tuple<buffer::Uniform *, pipeline::ResourceSet *> > render_data_pool;

							Kernel(Engine* e);
							~Kernel();
						};
						command::Buffer *primary_cmd = nullptr;
						sync::Semaphore *semaphore = nullptr;
						pipeline::Resource *pipeline_resource = nullptr;
						bool input_texture_changed = false;
						std::vector<Kernel> kernels;

						Frame(Engine* e);
						~Frame();
					};

					std::vector<Frame> frames;
					/// 0 is diffuse environment
					/// 1 is specular environment
					/// 2 is brdflut
					/// 3 is ambient occlusion
					/// then other shadow maps
					std::vector<std::shared_ptr<texture::Texture> > input_textures;

					/// If we are in a weak device it's gonna use only one shadow mapper.
					/// Only is debug mode it gonna check for this
					bool weak_device = true;

					bool diffuse_environment_has_provider = false;
					bool specular_environment_has_provider = false;
					bool brdflut_has_provider = false;
					bool ambient_occlusion_has_provider = false;

				public:
					ForwardPbr(Engine* e);
					~ForwardPbr();

					void set_shadow_mapper(std::shared_ptr<texture::Texture2D> texture, unsigned int shadow_index = 0);
					void set_diffuse_environment(std::shared_ptr<texture::Texture2D> texture);
					void set_specular_environment(std::shared_ptr<texture::Cube> texture);
					void set_brdflut(std::shared_ptr<texture::Texture2D> texture);
					void set_ambient_occlusion(std::shared_ptr<texture::Texture2D> texture);

					void set_diffuse_environment_has_provider(bool b);
					void set_specular_environment_has_provider(bool b);
					void set_brdflut_has_provider(bool b);
					void set_ambient_occlusion_has_provider(bool b);
				};
			}
		}
	}
}
#endif