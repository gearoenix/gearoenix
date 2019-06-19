#include "cr-gr-node.hpp"
#include "../asset/cr-asset-manager.hpp"

gearoenix::core::graph::Node::Node(const std::vector<std::string>& input_links, const std::vector<std::string>& output_links) noexcept
    : asset::Asset(asset::Manager::create_id(), asset::Type::NODE)
{
    input_links_providers_links.resize(input_links.size());
    for (unsigned int i = 0; i < input_links.size(); ++i) {
        input_links_string_index[input_links[i]] = i;
    }
    output_links_consumers_links.resize(output_links.size());
    for (unsigned int i = 0; i < output_links.size(); ++i) {
        output_links_string_index[output_links[i]] = i;
    }
}

void gearoenix::core::graph::Node::set_provider(const unsigned int input_link_index, const std::shared_ptr<Node>& o, const unsigned int provider_output_link_index) noexcept
{
    input_links_providers_links[input_link_index] = std::make_pair(o, provider_output_link_index);
}

void gearoenix::core::graph::Node::remove_provider(const unsigned int input_link_index) noexcept
{
    auto& p = input_links_providers_links[input_link_index];
    const std::shared_ptr<Node> ptr = p.first;
    if (ptr == nullptr)
        return;
    const unsigned int l = p.second;
    p = std::make_pair(nullptr, 0);
    ptr->remove_consumer(l, asset_id, input_link_index);
}

void gearoenix::core::graph::Node::set_consumer(const unsigned int output_link_index, const std::shared_ptr<Node>& o, const unsigned int consumer_input_link_index) noexcept
{
    output_links_consumers_links[output_link_index][std::make_pair(o->get_asset_id(), consumer_input_link_index)] = o;
}

void gearoenix::core::graph::Node::remove_consumer(const unsigned int output_link_index, const Id node_id, const unsigned int consumer_input_link_index) noexcept
{
    auto& m = output_links_consumers_links[output_link_index];
    auto search = m.find(std::make_pair(node_id, consumer_input_link_index));
    if (search == m.end())
        return;
    const std::shared_ptr<Node> ptr = search->second.lock();
    const unsigned int l = search->first.second;
    m.erase(search);
    ptr->remove_provider(l);
}

unsigned int gearoenix::core::graph::Node::get_input_link_index(const std::string& name, bool& exist) const noexcept
{
    auto a = input_links_string_index.find(name);
    if (a == input_links_string_index.end()) {
        exist = false;
        return 0;
    }
    exist = true;
    return a->second;
}

unsigned int gearoenix::core::graph::Node::get_output_link_index(const std::string& name, bool& exist) const noexcept
{
    auto a = output_links_string_index.find(name);
    if (a == output_links_string_index.end()) {
        exist = false;
        return 0;
    }
    exist = true;
    return a->second;
}

const std::vector<std::string> gearoenix::core::graph::Node::get_input_links_names() const noexcept
{
    std::vector<std::string> result;
    for (const auto& p : input_links_string_index) {
        result.push_back(p.first);
    }
    return result;
}

const std::vector<std::string> gearoenix::core::graph::Node::get_output_links_names() const noexcept
{
    std::vector<std::string> result;
    for (const auto& p : output_links_string_index) {
        result.push_back(p.first);
    }
    return result;
}

void gearoenix::core::graph::Node::connect(const std::shared_ptr<Node>& p, const unsigned int po, const std::shared_ptr<Node>&c, const unsigned int ci) noexcept
{
	p->set_consumer(po, c, ci);
	c->set_provider(ci, p, po);
}
