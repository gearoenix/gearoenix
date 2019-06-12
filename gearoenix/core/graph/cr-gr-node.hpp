#ifndef GEAROENIX_CORE_GR_NODE_HPP
#define GEAROENIX_CORE_GR_NODE_HPP
#include "../asset/cr-asset.hpp"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
namespace gearoenix::core::graph {
class Node : public asset::Asset {
private:
protected:
    std::map<std::string, unsigned int> input_links_string_index;
    std::map<std::string, unsigned int> output_links_string_index;
    std::vector<std::pair<std::shared_ptr<Node>, unsigned int>> input_links_providers_links;
    std::vector<std::map<core::Id, std::pair<std::weak_ptr<Node>, unsigned int>>> output_links_consumers_links;
    Node(const std::vector<std::string>& input_links, const std::vector<std::string>& output_links) noexcept;

public:
    virtual ~Node() noexcept = default;
	virtual void set_provider(unsigned int input_link_index, const std::shared_ptr<Node>& o, unsigned int provider_output_link_index) noexcept;
	virtual void remove_provider(unsigned int input_link_index) noexcept;
	virtual void set_consumer(unsigned int output_link_index, const std::shared_ptr<Node>& o, unsigned int consumer_input_link_index) noexcept;
	virtual void remove_cunsomer(unsigned int output_link_index, Id node_id) noexcept;
    unsigned int get_input_link_index(const std::string& name, bool& exist) const noexcept;
    unsigned int get_output_link_index(const std::string& name, bool& exist) const noexcept;
    const std::vector<std::string> get_input_links_names() const noexcept;
    const std::vector<std::string> get_output_links_names() const noexcept;
};
}
#endif