#include "cr-gr-node.hpp"

gearoenix::core::graph::node::Node::Node() {}

gearoenix::core::graph::node::Node::~Node() {}

void gearoenix::core::graph::node::Node::set_provider(unsigned int input_link_index, const std::shared_ptr<Node>& o) 
{
	providers[input_index] = o;
}

void gearoenix::core::graph::node::Node::set_consumer(unsigned int output_link_index, const std::weak_ptr<Node>& o)
{
	consumers[input_index].insert(o);
}

unsigned int gearoenix::core::graph::node::Node::get_input_link_index(const std::string &name)
{
	input_links_string_index
}

unsigned int gearoenix::core::graph::node::Node::get_output_link_index(const std::string &name);