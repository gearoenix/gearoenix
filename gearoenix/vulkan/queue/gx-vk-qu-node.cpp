#include "gx-vk-qu-node.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

gearoenix::vulkan::queue::Node::Node(engine::Engine& e, std::string name, VkPipelineStageFlags stage) noexcept
    : name(std::move(name))
    , stage(stage)
    , cmds(e.get_command_manager().create_frame_based())
{
}

gearoenix::vulkan::queue::Node::~Node() noexcept = default;

void gearoenix::vulkan::queue::Node::clear_traversing_level() noexcept
{
    traversal_level = -1;
    traversed = false;
    for (auto& c : consumers)
        c.second.first->clear_traversing_level();
}

int gearoenix::vulkan::queue::Node::update_traversing_level(const int tl) noexcept
{
    int max_tl = tl;
    if (tl > traversal_level) {
        traversal_level = tl;
        for (auto& c : consumers) {
            const auto child_max_tl = c.second.first->update_traversing_level(tl + 1);
            if (child_max_tl > max_tl)
                max_tl = child_max_tl;
        }
    }
    return max_tl;
}

void gearoenix::vulkan::queue::Node::connect(
    Node* const provider,
    Node* const consumer,
    engine::Engine& e) noexcept
{
    GX_CHECK_EQUAL_D(consumer->providers.end(), consumer->providers.find(provider->name))
    GX_CHECK_EQUAL_D(provider->consumers.end(), provider->consumers.find(consumer->name))

    auto semaphores = sync::Semaphore::create_frame_based(e);

    consumer->providers.emplace(provider->name, std::make_pair(provider, semaphores));
    provider->consumers.emplace(consumer->name, std::make_pair(consumer, std::move(semaphores)));
}

#endif