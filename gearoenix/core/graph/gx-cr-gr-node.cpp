#include "gx-cr-gr-node.hpp"
#include "../asset/gx-cr-asset-manager.hpp"

gearoenix::core::graph::Node::Node(std::string n, const std::vector<std::string>& input_links, const std::vector<std::string>& output_links) noexcept
    : asset::Asset(asset::Manager::create_id(), asset::Type::Node, std::move(n))
{
    input_links_providers_links.resize(input_links.size());
    for (std::size_t i = 0; i < input_links.size(); ++i) {
        input_links_string_index[input_links[i]] = static_cast<unsigned int>(i);
    }
    output_links_consumers_links.resize(output_links.size());
    for (std::size_t i = 0; i < output_links.size(); ++i) {
        output_links_string_index[output_links[i]] = static_cast<unsigned int>(i);
    }
}

gearoenix::core::graph::Node::~Node() noexcept = default;

void gearoenix::core::graph::Node::set_provider(const unsigned int input_link_index, Node* const o, const unsigned int provider_output_link_index) noexcept
{
    input_links_providers_links[input_link_index] = std::make_pair(o, provider_output_link_index);
}

void gearoenix::core::graph::Node::clear_provider(unsigned int input_link_index) noexcept
{
    input_links_providers_links[input_link_index] = std::make_pair(nullptr, 0);
}

void gearoenix::core::graph::Node::set_providers_count(const std::size_t count) noexcept
{
    input_links_providers_links.resize(count);
}

void gearoenix::core::graph::Node::remove_provider(const unsigned int input_link_index) noexcept
{
    auto& p = input_links_providers_links[input_link_index];
    Node* const ptr = p.first;
    if (ptr == nullptr)
        return;
    const unsigned int l = p.second;
    p = std::make_pair(nullptr, 0);
    ptr->remove_consumer(l, id, input_link_index);
}

void gearoenix::core::graph::Node::set_consumer(const unsigned int output_link_index, Node* const o, const unsigned int consumer_input_link_index) noexcept
{
    output_links_consumers_links[output_link_index][std::make_pair(o->get_id(), consumer_input_link_index)] = o;
}

void gearoenix::core::graph::Node::remove_consumer(const unsigned int output_link_index, const Id node_id, const unsigned int consumer_input_link_index) noexcept
{
    auto& m = output_links_consumers_links[output_link_index];
    auto search = m.find(std::make_pair(node_id, consumer_input_link_index));
    if (search == m.end())
        return;
    Node* const ptr = search->second;
    const unsigned int l = search->first.second;
    m.erase(search);
    ptr->remove_provider(l);
}

unsigned int gearoenix::core::graph::Node::get_input_link_index(const std::string& n, bool& exist) const noexcept
{
    auto a = input_links_string_index.find(n);
    if (a == input_links_string_index.end()) {
        exist = false;
        return 0;
    }
    exist = true;
    return a->second;
}

unsigned int gearoenix::core::graph::Node::get_output_link_index(const std::string& n, bool& exist) const noexcept
{
    auto a = output_links_string_index.find(n);
    if (a == output_links_string_index.end()) {
        exist = false;
        return 0;
    }
    exist = true;
    return a->second;
}

std::vector<std::string> gearoenix::core::graph::Node::get_input_links_names() const noexcept
{
    std::vector<std::string> result;
    for (const auto& p : input_links_string_index) {
        result.push_back(p.first);
    }
    return result;
}

std::vector<std::string> gearoenix::core::graph::Node::get_output_links_names() const noexcept
{
    std::vector<std::string> result;
    for (const auto& p : output_links_string_index) {
        result.push_back(p.first);
    }
    return result;
}

void gearoenix::core::graph::Node::connect(Node* const p, const unsigned int po, Node* const c, const unsigned int ci) noexcept
{
    p->set_consumer(po, c, ci);
    c->set_provider(ci, p, po);
}
