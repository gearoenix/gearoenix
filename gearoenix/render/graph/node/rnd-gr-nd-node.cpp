#include "rnd-gr-nd-node.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"

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
}

void gearoenix::render::graph::node::Node::set_provider(const unsigned int input_link_index, const std::shared_ptr<core::graph::Node>& o, const unsigned int provider_output_link_index) noexcept {
	Node::set_provider(input_link_index, o, provider_output_link_index);
}

void gearoenix::render::graph::node::Node::set_input_texture(const std::shared_ptr<texture::Texture>& t, const unsigned int index) noexcept
{
    input_textures[index] = t;
}

void gearoenix::render::graph::node::Node::set_render_target(const std::shared_ptr<texture::Target>& t) noexcept
{
    render_target = t;
}