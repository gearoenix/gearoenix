#ifndef GEAROENIX_CORE_GR_NODE_HPP
#define GEAROENIX_CORE_GR_NODE_HPP
#include <string>
#include <memory>
#include <set>
namespace gearoenix {
	namespace core {
		namespace graph {
			class Node {
			protected:
				std::map<std::string, unsigned int> input_links_string_index;
				std::map<std::string, unsigned int> output_links_string_index;
				std::vector<std::shared_ptr<Node> > providers;
				std::vector<std::set<std::weak_ptr<Node> > > consumers;
			public:
				Node();
				virtual ~Node();
				virtual void set_provider(unsigned int input_link_index, const std::shared_ptr<Node>& o);
				virtual void set_consumer(unsigned int output_link_index, const std::weak_ptr<Node>& o);
				unsigned int get_input_link_index(const std::string &name);
				unsigned int get_output_link_index(const std::string &name);
			};
		}
	}
}
#endif // GEAROENIX_CORE_GR_NODE_HPP
