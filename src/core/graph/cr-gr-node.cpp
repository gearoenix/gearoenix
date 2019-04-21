#include "cr-gr-node.hpp"

gearoenix::core::graph::Node::Node(const std::vector<std::string>& input_links, const std::vector<std::string>& output_links)
{

    providers.resize(input_links.size());
    for (unsigned int i = 0; i < input_links.size(); ++i) {
        input_links_string_index[input_links[i]] = i;
        providers[i] = nullptr;
    }
    consumers.resize(output_links.size());
    for (unsigned int i = 0; i < output_links.size(); ++i) {
        output_links_string_index[output_links[i]] = i;
        consumers[i] = std::set<std::weak_ptr<Node>, weak_less<std::weak_ptr<Node>>>();
    }
}

gearoenix::core::graph::Node::~Node() {}

void gearoenix::core::graph::Node::set_provider(unsigned int input_link_index, const std::shared_ptr<Node>& o)
{
    providers[input_link_index] = o;
}

void gearoenix::core::graph::Node::set_consumer(unsigned int output_link_index, const std::weak_ptr<Node>& o)
{
    consumers[output_link_index].insert(o);
}

unsigned int gearoenix::core::graph::Node::get_input_link_index(const std::string& name, bool& exist) const
{
    auto a = input_links_string_index.find(name);
    if (a == input_links_string_index.end()) {
        exist = false;
        return 0;
    }
    exist = true;
    return a->second;
}

unsigned int gearoenix::core::graph::Node::get_output_link_index(const std::string& name, bool& exist) const
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
    for (const auto& p : input_links_string_index) {
        result.push_back(p.first);
    }
    return result;
}

const std::vector<std::string> gearoenix::core::graph::Node::get_output_links_names() const
{
    std::vector<std::string> result;
    for (const auto& p : output_links_string_index) {
        result.push_back(p.first);
    }
    return result;
}
