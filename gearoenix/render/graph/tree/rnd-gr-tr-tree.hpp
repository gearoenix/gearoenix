#ifndef GEAROEMIX_RENDER_GRAPH_TREE_TREE_HPP
#define GEAROEMIX_RENDER_GRAPH_TREE_TREE_HPP
#include <memory>
namespace gearoenix {
	namespace render {
		namespace engine {
			class Engine;
		}
		namespace graph {
			namespace tree {
				class Tree {
				protected:
					const std::shared_ptr<engine::Engine> e;
					Tree(
						const std::shared_ptr<engine::Engine>& e);
				public:
					virtual ~Tree();
					/// Multithreaded rendering happens in here
					virtual void record(const unsigned int kernel_index) = 0;
					/// Submit is called from graphic thread
					virtual void submit() = 0;
				};
			}
		}
	}
}
#endif