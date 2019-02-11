#ifndef GEAROEMIX_RENDER_GRAPH_NODE_NODE_HPP
#define GEAROEMIX_RENDER_GRAPH_NODE_NODE_HPP
#include "../../../core/graph/cr-gr-node.hpp"

/// Node is reponsible to record command buffers.
/// It gathers all the needed data like pipeline, descriptor-sets, ... .
/// Each node has input and output links.
/// Node must provide render-data that is contain none-shared objects, like command buffer, semaphores, ...
/// There are some predifined ids for links, in addition users of engine can specify their ids
/// Node must bring a structure for its output link, and must accept a trait as its dependancy
/// Node will contain strong pointer for its providers and weak pointer for its consumers
/// A node must register itself a cunsumer it is, gather its data when provider provided its data.
/// A node must register its cunsumers and signal them whenever their needed data is gathered
/// So do not forget provider is responsible for signaling its registered consumers
/// and consumers are responsible for waiting on signal of provider for using data
/// Each node will get data of the light, scene, camera, model, ...
/// Always there is and must be a single provider for each dependancy
/// But there might be several or zero consumer for a data
/// This system can be used to push independant commands on separate queue

namespace gearoenix {
	namespace render {
		class Engine;
		namespace graph {
			namespace node {
				class Node : public core::graph::Node {
				protected:
					Node(Engine* e);

				public:
					virtual ~Node();
				};
			}
		}
	}
}
#endif