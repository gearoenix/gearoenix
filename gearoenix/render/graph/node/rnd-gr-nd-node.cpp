#include "rnd-gr-nd-node.hpp"
#include "../../rnd-engine.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"

gearoenix::render::graph::node::Node::Node(
	Engine * e,
	const pipeline::PipelineType::Id pipeline_type_id,
	const unsigned int input_textures_count,
	const unsigned int output_textures_count,
	const std::vector<std::string> &input_links,
	const std::vector<std::string> &output_links,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> call) :
	core::graph::Node(input_links, output_links),
	e(e),
	render_pipeline(e->get_pipeline_manager()->get(pipeline_type_id, call))
{
	input_textures.resize(input_textures_count);
	for (unsigned int i = 0; i < input_textures_count; ++i) {
		input_textures.push_back(nullptr);
	}
	output_textures.resize(output_textures_count);
	for (unsigned int i = 0; i < output_textures_count; ++i) {
		output_textures.push_back(nullptr);
	}
}

gearoenix::render::graph::node::Node::~Node()
{
}

void gearoenix::render::graph::node::Node::set_input_texture(const std::shared_ptr<texture::Texture>& t, const unsigned int index)
{
	input_textures[index] = t;
}
