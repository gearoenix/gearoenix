#include "rnd-gr-nd-forward-pbr-directional.hpp"
#include "../../rnd-engine.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../material/rnd-mat-material.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../texture/rnd-txt-texture-2d.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../model/rnd-mdl-model.hpp"
#include <thread>

gearoenix::render::graph::node::ForwardPbrDirectional::ForwardPbrDirectional(
	Engine * e, core::sync::EndCaller<core::sync::EndCallerIgnore> call) :
	Node(
		e,
		pipeline::PipelineType::ForwardPbrDirectionalShadow,
		5,
		1,
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

void gearoenix::render::graph::node::ForwardPbrDirectional::set_input_texture(
	const std::shared_ptr<texture::Texture> &t, const unsigned int index)
{
	Node::set_input_texture(t, index);
	for (ForwardPbrDirectionalFrame &f : frames) {
		f.input_texture_changed = true;
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectional::update()
{
	const unsigned int frame_number = e->get_frame_number();
	ForwardPbrDirectionalFrame &frame = frames[frame_number];
	if (frame.input_texture_changed) {
		frame.input_texture_changed = false;
		frame.pipeline_resource = e->get_pipeline_manager()->create_resource(input_textures);
	}
	frame.primary_cmd->begin();
	for (ForwardPbrDirectionalKernel &kernel : frame.kernels) {
		kernel.latest_render_data_pool = 0;
		kernel.secondary_cmd->begin();
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectional::record(
	const std::shared_ptr<scene::Scene> &s,
	const std::shared_ptr<camera::Camera> & c,
	const std::shared_ptr<light::Directional> & l,
	const std::shared_ptr<model::Model> & m,
	const unsigned int kernel_index)
{
	const unsigned int frame_number = e->get_frame_number();
	ForwardPbrDirectionalFrame &frame = frames[frame_number];
	ForwardPbrDirectionalKernel &kernel = frame.kernels[kernel_index];
	const std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material> > > &meshes = m->get_meshes();
	for (const std::pair<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material> > > &id_mesh_material : meshes) {
		const std::shared_ptr<mesh::Mesh> &msh = std::get<0>(id_mesh_material.second);
		const std::shared_ptr < material::Material> &mat = std::get<1>(id_mesh_material.second);
		if (kernel.latest_render_data_pool >= kernel.render_data_pool.size()) {
			kernel.render_data_pool.push_back(std::make_tuple(
				std::shared_ptr<buffer::Uniform>(e->get_buffer_manager()->create_uniform(sizeof(ForwardPbrDirectionalUniform))),
				std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet>(render_pipeline->create_resource_set())));
		}
		std::tuple<std::shared_ptr<buffer::Uniform>, std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet> > &pool = 
			kernel.render_data_pool[kernel.latest_render_data_pool];
		std::shared_ptr<buffer::Uniform> &ub = std::get<0>(pool);
		std::shared_ptr <pipeline::ForwardPbrDirectionalShadowResourceSet> &prs = std::get<1>(pool);
		ForwardPbrDirectionalUniform us;
		us.mvp = c.get_view_projection_matrix() * m.get_model_matrix();
		us.light_view_projection_biases = l.get_view_projection_biases();
		us.light_color = l.get_color().extend(1.0);
		ub->update(&us);
		prs->set_scene(s);
		prs->set_camera(c);
		prs->set_light(l);
		prs->set_model(m);
		prs->set_mesh(msh);
		prs->set_material(mat);
		prs->set_effect(ub, prs);
		prs->record(kernel.secondary_cmd);
		++kernel.latest_render_data_pool;
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectional::submit()
{
	const unsigned int frame_number = e->get_frame_number();
	ForwardPbrDirectionalFrame &frame = frames[frame_number];
	std::shared_ptr<command::Buffer> &cmd = frame.primary_cmd;
	render_pipeline->bind(cmd);
	render_target->bind(cmd);
	for (const ForwardPbrDirectionalKernel k : frame.kernels) {
		cmd->record(k.secondary_cmd);
	}
	std::vector<std::shared_ptr<sync::Semaphore> > pss(providers.size());
	for (const std::shared_ptr<core::graph::Node> &p : providers) {
		if (p == nullptr) continue;
		const std::shared_ptr<Node> &rp = std::static_pointer_cast<Node>(p);
		if (rp == nullptr) continue;
		const std::shared_ptr<sync::Semaphore> &ps = rp->get_semaphore(frame_number);
		if (ps == nullptr) continue;
		pss.push_back(ps);
	}
	e->submit(pss, cmd, frame.semaphore);
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

gearoenix::render::graph::node::ForwardPbrDirectionalKernel::ForwardPbrDirectionalKernel(Engine *e, const unsigned int kernel_index) :
	secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}
