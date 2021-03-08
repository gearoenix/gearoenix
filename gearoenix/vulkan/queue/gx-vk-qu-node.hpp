#ifndef GEAROENIX_VULKAN_QUEUE_NODE_HPP
#define GEAROENIX_VULKAN_QUEUE_NODE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan::queue {
struct Node final {
    const std::string name;
    const VkPipelineStageFlags wait_stage;
    const std::vector<std::shared_ptr<command::Buffer>> cmds;
    int traversal_level = -1;
    bool traversed = false;
    // semaphores belong to providers and only for performance they have been added in here
    std::map<std::string, std::pair<Node*, const std::vector<std::shared_ptr<sync::Semaphore>>>> providers;
    std::map<std::string, std::pair<Node*, const std::vector<std::shared_ptr<sync::Semaphore>>>> consumers;

    explicit Node(engine::Engine&, std::string name, VkPipelineStageFlags) noexcept;

    ~Node() noexcept;
    void clear_traversing_level() noexcept;
    void update_traversing_level(int) noexcept;
    static void connect(Node* provider, Node* consumer, engine::Engine& e) noexcept;
};
}

#endif
#endif