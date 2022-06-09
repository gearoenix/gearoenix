#include "gx-vk-qu-graph.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include "gx-vk-qu-queue.hpp"

gearoenix::vulkan::queue::Graph::Graph(engine::Engine& e) noexcept
    : start_semaphore(sync::Semaphore::create_frame_based(e))
    , end_semaphore(sync::Semaphore::create_frame_based(e))
    , fence(sync::Fence::create_frame_based(e, true))
    , start(&nodes.emplace(Queue::START_FRAME, Node(e, Queue::START_FRAME, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)).first->second)
    , end(&nodes.emplace(Queue::END_FRAME, Node(e, Queue::END_FRAME, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)).first->second)
{
    Node::connect(start, end, e);
}

gearoenix::vulkan::queue::Graph::~Graph() noexcept = default;

void gearoenix::vulkan::queue::Graph::update_traversing_level() noexcept
{
    start->clear_traversing_level();
    const auto max_tl = start->update_traversing_level(0);
    submit_data.resize(static_cast<std::size_t>(max_tl));
}

void gearoenix::vulkan::queue::Graph::clear_submit_data() noexcept
{
    for (auto& d : submit_data) {
        auto& [ws, ps, cs, ss] = d;
        ws.clear();
        ps.clear();
        cs.clear();
        ss.clear();
    }
}

void gearoenix::vulkan::queue::Graph::update_submit_data(Node& n, const std::size_t frame_number) noexcept
{
    if (n.traversed)
        return;
    n.traversed = true;
    while (static_cast<std::size_t>(n.traversal_level) + 1 > submit_data.size())
        submit_data.emplace_back();
    auto& [ws, ps, cs, ss] = submit_data[n.traversal_level];
    for (auto& p : n.providers) {
        auto& pd = p.second;
        ws.emplace_back(pd.second[frame_number]->get_vulkan_data());
        ps.push_back(pd.first->stage);
    }
    cs.emplace_back(n.cmds[frame_number]->get_vulkan_data());
    for (auto& c : n.consumers) {
        ss.emplace_back(c.second.second[frame_number]->get_vulkan_data());
    }
    for (auto& c : n.consumers) {
        update_submit_data(*c.second.first, frame_number);
    }
}

void gearoenix::vulkan::queue::Graph::update(const std::size_t frame_number) noexcept
{
    update_traversing_level();
    clear_submit_data();
    update_submit_data(*start, frame_number);
}

#endif