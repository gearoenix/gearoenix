#include "cr-gr-node.hpp"

gearoenix::core::graph::Node::Node() {}

gearoenix::core::graph::Node::~Node() {}

void gearoenix::core::graph::Node::set_provider(unsigned int input_link_index, const std::shared_ptr<Node>& o) 
{
	providers[input_link_index] = o;
}

void gearoenix::core::graph::Node::set_consumer(unsigned int output_link_index, const std::weak_ptr<Node>& o)
{
	consumers[output_link_index].insert(o);
}

unsigned int gearoenix::core::graph::Node::get_input_link_index(const std::string &name, bool &exist) const
{
	auto a = input_links_string_index.find(name);
	if (a == input_links_string_index.end()) {
		exist = false;
		return 0;
	}
	exist = true;
	return a->second;
}

unsigned int gearoenix::core::graph::Node::get_output_link_index(const std::string &name, bool &exist) const
{
	auto a = output_links_string_index.find(name);
	if (a == output_links_string_index.end()) {
		exist = false;
		return 0;
	}
	exist = true;
	return a->second;
}

const std::vector<std::string> gearoenix::core::graph::Node::get_input_links_names() const
{
	std::vector<std::string> result;
	for (const auto &p : input_links_string_index) {
		result.push_back(p.first);
	}
	return result;
}

const std::vector<std::string> gearoenix::core::graph::Node::get_output_links_names() const
{
	std::vector<std::string> result;
	for (const auto &p : output_links_string_index) {
		result.push_back(p.first);
	}
	return result;
}
