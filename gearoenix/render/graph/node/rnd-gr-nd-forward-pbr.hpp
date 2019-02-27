#ifndef GEAROEMIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#define GEAROEMIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#include "rnd-gr-nd-node.hpp"
#include <vector>

namespace gearoenix {
	namespace render {
		namespace command {
			class Buffer;
		}
		namespace pipeline {
			class ResourceSet;
		}
		namespace sync {
			class Semaphore;
		}
		namespace graph {
			namespace node {
				class ForwardPbr : public Node {
				private:
					struct Frame {
						struct Kernel {
							command::Buffer *secondary_cmd = nullptr;
							unsigned int latest_pipeline_resource_set_pool = 0;
							std::vector<pipeline::ResourceSet *> pipeline_resource_set_pool;
						};
						command::Buffer *primary_cmd = nullptr;
						sync::Semaphore *semaphore = nullptr;
						std::vector<Kernel> kernels;

					};
				protected:
					ForwardPbr(Engine* e);
				public:
					virtual ~ForwardPbr();
				};
			}
		}
	}
}
#endif