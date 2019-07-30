#include "rnd-gr-nd-node.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"

gearoenix::render::graph::node::Node::Node(
    engine::Engine* const e,
    const pipeline::Type pipeline_type_id,
    const unsigned int input_textures_count,
    const unsigned int output_textures_count,
    const std::vector<std::string>& input_links,
    const std::vector<std::string>& output_links,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : core::graph::Node(input_links, output_links)
    , e(e)
{
    const auto frames_count = e->get_frames_count();
    const auto& cmd_mgr = e->get_command_manager();
    core::sync::EndCaller<pipeline::Pipeline> pipcall([call](std::shared_ptr<pipeline::Pipeline>) {});
    render_pipeline = e->get_pipeline_manager()->get(pipeline_type_id, pipcall);
    input_textures.resize(input_textures_count);
    for (unsigned int i = 0; i < input_textures_count; ++i) {
        input_textures[i] = nullptr;
    }
    output_textures.resize(output_textures_count);
    for (unsigned int i = 0; i < output_textures_count; ++i) {
        output_textures[i] = nullptr;
    }
    link_providers_frames_semaphores.resize(input_links.size());
    links_consumers_frames_semaphores.resize(output_links.size());
    update_next_semaphores();
    update_previous_semaphores();
    frames_primary_cmd.resize(frames_count);
    for (auto& cmd : frames_primary_cmd)
        cmd = cmd_mgr->create_primary_command_buffer();
}

void gearoenix::render::graph::node::Node::update_next_semaphores() noexcept
{
    const auto fc = e->get_frames_count();
    nxt_sems.resize(fc);
    for (int i = 0; i < fc; ++i) {
        auto& s = nxt_sems[i];
        for (auto& l : links_consumers_frames_semaphores)
            for (auto& c : l)
                s.push_back(c.second[i]);
    }
}

void gearoenix::render::graph::node::Node::update_previous_semaphores() noexcept
{
    const auto fc = e->get_frames_count();
    pre_sems.resize(fc);
    for (int i = 0; i < fc; ++i) {
        auto& s = pre_sems[i];
        for (auto& p : link_providers_frames_semaphores) {
            if (p.size() != fc)
                continue;
            s.push_back(p[i]);
        }
    }
}

void gearoenix::render::graph::node::Node::set_provider(const unsigned int input_link_index, core::graph::Node*const o, const unsigned int provider_output_link_index) noexcept
{
    auto& cur = input_links_providers_links[input_link_index];
    /// This is for better performance in shadow assignment.
    if (cur.first == o && cur.second == provider_output_link_index)
        return;
    core::graph::Node::set_provider(input_link_index, o, provider_output_link_index);
    link_providers_frames_semaphores[input_link_index] = dynamic_cast<Node*>(o)->get_link_frames_semaphore(
        provider_output_link_index, asset_id, input_link_index);
    /// This is because of flexibility in frames-count, and it does'nt happen very offen
    update_previous_semaphores();
}

void gearoenix::render::graph::node::Node::remove_provider(const unsigned int input_link_index) noexcept
{
    link_providers_frames_semaphores[input_link_index].clear();
    core::graph::Node::remove_provider(input_link_index);
    update_previous_semaphores();
}

void gearoenix::render::graph::node::Node::remove_consumer(const unsigned int output_link_index, const core::Id node_id, const unsigned int consumer_input_link_index) noexcept
{
    links_consumers_frames_semaphores[output_link_index][std::make_pair(node_id, consumer_input_link_index)].clear();
    core::graph::Node::remove_consumer(output_link_index, node_id, consumer_input_link_index);
    update_next_semaphores();
}

gearoenix::render::texture::Texture* gearoenix::render::graph::node::Node::get_output_texture(unsigned int index) const noexcept
{
    return output_textures[index];
}

void gearoenix::render::graph::node::Node::set_input_texture(texture::Texture*const t, const unsigned int index) noexcept
{
    input_textures[index] = t;
}

void gearoenix::render::graph::node::Node::set_render_target(const std::shared_ptr<texture::Target>& t) noexcept
{
    render_target = t;
}

void gearoenix::render::graph::node::Node::update() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    frames_primary_cmd[frame_number]->begin();
}

void gearoenix::render::graph::node::Node::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    auto& pre = pre_sems[frame_number];
    auto& nxt = nxt_sems[frame_number];
    command::Buffer* cmd = frames_primary_cmd[frame_number];
    e->submit(pre.size(), pre.data(), 1, &cmd, nxt.size(), nxt.data());
}

const std::vector<gearoenix::render::sync::Semaphore*> gearoenix::render::graph::node::Node::get_link_frames_semaphore(
    const unsigned int output_link_index,
    const core::Id consumer_id,
    const unsigned int consumer_input_link_index) noexcept
{
    auto& v = links_consumers_frames_semaphores[output_link_index][std::make_pair(consumer_id, consumer_input_link_index)];
    const auto s = e->get_frames_count();
    if (v.size() != s) {
        v.resize(s);
        for (auto i = 0; i < s; ++i)
            v[i] = e->create_semaphore();
        /// This is because of flexibility in frames-count, and it does'nt happen very offen
        update_next_semaphores();
    }
    return v;
}
