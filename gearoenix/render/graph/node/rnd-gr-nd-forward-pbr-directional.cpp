#include "rnd-gr-nd-forward-pbr-directional.hpp"
#include "../../rnd-engine.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../texture/rnd-txt-texture-2d.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"
#include <thread>

gearoenix::render::graph::node::ForwardPbrDirectional::ForwardPbrDirectional(Engine * e, core::sync::EndCaller<core::sync::EndCallerIgnore> call) :
	Node(
		e,
		pipeline::PipelineType::ForwardPbrDirectionalShadow,
		5,
		0,
		{
			"diffuse environment"
			"specular environment"
			"ambient occlusion"
			"shadow map"
		},
		{
			"color"
		},
		call
	)
{
	frames.resize(GX_FRAMES_COUNT);
	for (unsigned int i = 0; i < GX_FRAMES_COUNT; ++i)
	{
		frames.push_back(ForwardPbrDirectionalFrame(e));
	}
}

gearoenix::render::graph::node::ForwardPbrDirectional::~ForwardPbrDirectional()
{
}

void gearoenix::render::graph::node::ForwardPbrDirectional::set_diffuse_environment(const std::shared_ptr<texture::Cube> &t)
{
	set_input_texture(std::static_pointer_cast<texture::Texture>(t), 0);
}

void gearoenix::render::graph::node::ForwardPbrDirectional::set_specular_environment(const std::shared_ptr<texture::Cube> &t)
{
	set_input_texture(std::static_pointer_cast<texture::Texture>(t), 1);
}

void gearoenix::render::graph::node::ForwardPbrDirectional::set_ambient_occlusion(const std::shared_ptr<texture::Texture2D> &t)
{
	set_input_texture(t, 2);
}

void gearoenix::render::graph::node::ForwardPbrDirectional::set_shadow_mapper(const std::shared_ptr<texture::Texture2D> &t)
{
	set_input_texture(t, 3);
}

void gearoenix::render::graph::node::ForwardPbrDirectional::set_brdflut(const std::shared_ptr<texture::Texture2D> &t)
{
	set_input_texture(t, 4);
}

void gearoenix::render::graph::node::ForwardPbrDirectional::set_input_texture(const std::shared_ptr<texture::Texture> &t, const unsigned int index)
{
	Node::set_input_texture(t, index);
	for (ForwardPbrDirectionalFrame &f: frames) {
		f.input_texture_changed = true;
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectional::update()
{
	const unsigned int frame_number = e->get_frame_number();
	ForwardPbrDirectionalFrame &frame = frames[frame_number];
	if (frame.input_texture_changed) {
		frame.input_texture_changed = false;
		frame.pipeline_resource = render_pipeline->create_resource(input_textures);
	}
	frame.primary_cmd->begin();
	for (ForwardPbrDirectionalKernel &kernel : frame.kernels) {
		kernel.latest_render_data_pool = 0;
		kernel.secondary_cmd->begin();
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectional::record(const scene::Scene & s, const camera::Camera & c, const light::Directional& l, const model::Model & m, const unsigned int kernel_index)
{
	const unsigned int frame_number = e->get_frame_number();
	ForwardPbrDirectionalFrame &frame = frames[frame_number];
	ForwardPbrDirectionalKernel &kernel = frame.kernels[kernel_index];
	for (std::pair<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, material::Matrial> > &id_mesh_material : m.get_meshes()) {
		if (kernel.latest_render_data_pool >= kernel.render_data_pool.size()) {
			buffer::Uniform *u = e->get_buffer_manager()->create_uniform(sizeof(ForwardPbrDirectionalUniform));
			pipeline::ResourceSet *prs = render_pipeline->create_resource_set();
			kernel.render_data_pool.push_back(std::make_tuple(u, prs));
		}
		ForwardPbrDirectionalUniform us;
		/// todo
		/// fill the uniform buffer
		/// pass it to render pool data
		/// pass all pipeline resources to pipeline resource set
		/// record the command buffer
		++kernel.latest_render_data_pool;
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectional::submit()
{
}

gearoenix::render::graph::node::ForwardPbrDirectionalFrame::ForwardPbrDirectionalFrame(Engine *e) :
	primary_cmd(e->get_command_manager()->create_primary_command_buffer()),
	semaphore(e->create_semaphore())
{
	const unsigned int kernels_count = std::thread::hardware_concurrency();
	kernels.resize(kernels_count);
	for (unsigned int i = 0; i < kernels_count; ++i) {
		kernels.push_back(ForwardPbrDirectionalKernel(e, i));
	}
}

gearoenix::render::graph::node::ForwardPbrDirectionalFrame::~ForwardPbrDirectionalFrame()
{
	delete primary_cmd;
	primary_cmd = nullptr;
	delete semaphore;
	semaphore = nullptr;
}

gearoenix::render::graph::node::ForwardPbrDirectionalKernel::ForwardPbrDirectionalKernel(Engine *e, const unsigned int kernel_index) :
	secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::ForwardPbrDirectionalKernel::~ForwardPbrDirectionalKernel()
{
	delete secondary_cmd;
	secondary_cmd = nullptr;
	for (std::tuple<buffer::Uniform *, pipeline::ResourceSet *> &data : render_data_pool) {
		delete std::get<0>(data);
		delete std::get<1>(data);
	}
}
