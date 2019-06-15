#include "rnd-gr-nd-node.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"

gearoenix::render::graph::node::Node::Node(
    engine::Engine* const e,
    const pipeline::Type::Id pipeline_type_id,
    const unsigned int input_textures_count,
    const unsigned int output_textures_count,
    const std::vector<std::string>& input_links,
    const std::vector<std::string>& output_links,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : core::graph::Node(input_links, output_links)
    , e(e)
{
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
}

void gearoenix::render::graph::node::Node::update_next_semaphores() noexcept
{
    const auto fc = e->get_frames_count();
    nxt_sems.resize(fc);
    for (int i = 0; i < fc; ++i) {
        auto& s = nxt_sems[i];
        for (auto& l : links_consumers_frames_semaphores)
            for (auto& c : l)
                s.push_back(c.second[i].get());
    }
}

void gearoenix::render::graph::node::Node::update_previous_semaphores() noexcept
{
    const auto fc = e->get_frames_count();
    pre_sems.resize(fc);
    for (int i = 0; i < fc; ++i) {
        auto& s = pre_sems[i];
		for (auto& p : link_providers_frames_semaphores)
		{
			if (p.size() != fc) continue;
			s.push_back(p[i].get());
		}
    }
}

void gearoenix::render::graph::node::Node::set_provider(const unsigned int input_link_index, const std::shared_ptr<core::graph::Node>& o, const unsigned int provider_output_link_index) noexcept
{
    core::graph::Node::set_provider(input_link_index, o, provider_output_link_index);
    link_providers_frames_semaphores[input_link_index] = dynamic_cast<Node*>(o.get())->get_link_frames_semaphore(provider_output_link_index, asset_id);
    /// This is because of flexibility in frames-count, and it does'nt happen very offen
    update_previous_semaphores();
}

void gearoenix::render::graph::node::Node::remove_provider(const unsigned int input_link_index) noexcept
{
    link_providers_frames_semaphores[input_link_index].clear();
    core::graph::Node::remove_provider(input_link_index);
    update_previous_semaphores();
}

void gearoenix::render::graph::node::Node::remove_consumer(const unsigned int output_link_index, const core::Id node_id) noexcept
{
    links_consumers_frames_semaphores[output_link_index][node_id].clear();
    core::graph::Node::remove_consumer(output_link_index, node_id);
    update_next_semaphores();
}

void gearoenix::render::graph::node::Node::set_input_texture(const std::shared_ptr<texture::Texture>& t, const unsigned int index) noexcept
{
    input_textures[index] = t;
}

void gearoenix::render::graph::node::Node::set_render_target(const std::shared_ptr<texture::Target>& t) noexcept
{
    render_target = t;
}

const std::vector<std::shared_ptr<gearoenix::render::sync::Semaphore>> gearoenix::render::graph::node::Node::get_link_frames_semaphore(const unsigned int output_link_index, const core::Id consumer_id) noexcept
{
    auto& v = links_consumers_frames_semaphores[output_link_index][consumer_id];
    const auto s = e->get_frames_count();
    if (v.size() != s) {
        v.resize(s);
        for (auto i = 0; i < s; ++i)
            v[i] = std::shared_ptr<sync::Semaphore>(e->create_semaphore());
        /// This is because of flexibility in frames-count, and it does'nt happen very offen
        update_next_semaphores();
    }
    return v;
}
