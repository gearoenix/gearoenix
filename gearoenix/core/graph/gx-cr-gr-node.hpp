#ifndef GEAROENIX_CORE_GR_NODE_HPP
#define GEAROENIX_CORE_GR_NODE_HPP
#include "../asset/gx-cr-asset.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>
namespace gearoenix::core::graph {
class Node : public asset::Asset {
private:
protected:
    std::map<std::string, unsigned int> input_links_string_index;
    std::map<std::string, unsigned int> output_links_string_index;
    std::vector<std::pair<Node*, unsigned int>> input_links_providers_links;
    std::vector<std::map<std::pair<core::Id, unsigned int>, Node*>> output_links_consumers_links;
    Node(std::string name, const std::vector<std::string>& input_links, const std::vector<std::string>& output_links) noexcept;

public:
    ~Node() noexcept override;
    virtual void set_provider(unsigned int input_link_index, Node* o, unsigned int provider_output_link_index) noexcept;
    virtual void clear_provider(unsigned int input_link_index) noexcept;
    virtual void set_providers_count(std::size_t count) noexcept;
    virtual void remove_provider(unsigned int input_link_index) noexcept;
    virtual void set_consumer(unsigned int output_link_index, Node* o, unsigned int consumer_input_link_index) noexcept;
    virtual void remove_consumer(unsigned int output_link_index, Id node_id, unsigned int consumer_input_link_index) noexcept;
    unsigned int get_input_link_index(const std::string& name, bool& exist) const noexcept;
    unsigned int get_output_link_index(const std::string& name, bool& exist) const noexcept;
    [[nodiscard]] std::vector<std::string> get_input_links_names() const noexcept;
    [[nodiscard]] std::vector<std::string> get_output_links_names() const noexcept;
    static void connect(Node* p, unsigned int po, Node* c, unsigned int ci) noexcept;
};
}
#endif