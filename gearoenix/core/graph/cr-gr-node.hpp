#ifndef GEAROENIX_CORE_GR_NODE_HPP
#define GEAROENIX_CORE_GR_NODE_HPP
#include "../cr-types.hpp"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
namespace gearoenix::core::graph {
class Node {
private:
protected:
    std::map<std::string, unsigned int> input_links_string_index;
    std::map<std::string, unsigned int> output_links_string_index;
    std::vector<std::shared_ptr<Node>> providers;
    std::vector<std::vector<std::weak_ptr<Node>>> consumers;
    Node(const std::vector<std::string>& input_links, const std::vector<std::string>& output_links) noexcept;

public:
    virtual ~Node() noexcept = default;
    virtual void set_provider(unsigned int input_link_index, const std::shared_ptr<Node>& o) noexcept;
    virtual void set_consumer(unsigned int output_link_index, const std::weak_ptr<Node>& o) noexcept;
    unsigned int get_input_link_index(const std::string& name, bool& exist) const noexcept;
    unsigned int get_output_link_index(const std::string& name, bool& exist) const noexcept;
    const std::vector<std::string> get_input_links_names() const noexcept;
    const std::vector<std::string> get_output_links_names() const noexcept;
};
}
#endif // GEAROENIX_CORE_GR_NODE_HPP
