#include "rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-directional.hpp"
#include "../../material/rnd-mat-material.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../model/rnd-mdl-mesh.hpp"
#include "../../model/rnd-mdl-model.hpp"
#include "../../pipeline/rnd-pip-forward-pbr-directional-shadow.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../pipeline/rnd-pip-resource-set.hpp"
#include "../../pipeline/rnd-pip-resource.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../../texture/rnd-txt-texture-2d.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"
#include <thread>

gearoenix::render::graph::node::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(
	const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
	: Node(
		e,
		pipeline::Type::ForwardPbrDirectionalShadow,
		5,
		1,
		{ "diffuse environment"
		  "specular environment"
		  "ambient occlusion"
		  "shadow map" },
		{ "color" },
		call)
{
	frames.resize(GX_FRAMES_COUNT);
	for (unsigned int i = 0; i < GX_FRAMES_COUNT; ++i) {
		frames[i] = std::make_shared<ForwardPbrDirectionalShadowFrame>(e);
	}
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadow::~ForwardPbrDirectionalShadow()
{
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_diffuse_environment(const std::shared_ptr<texture::Cube>& t)
{
	set_input_texture(std::static_pointer_cast<texture::Texture>(t), 0);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_specular_environment(const std::shared_ptr<texture::Cube>& t)
{
	set_input_texture(std::static_pointer_cast<texture::Texture>(t), 1);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t)
{
	set_input_texture(t, 2);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t)
{
	set_input_texture(t, 3);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_brdflut(const std::shared_ptr<texture::Texture2D>& t)
{
	set_input_texture(t, 4);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_input_texture(
	const std::shared_ptr<texture::Texture>& t, const unsigned int index)
{
	Node::set_input_texture(t, index);
	for (const std::shared_ptr<ForwardPbrDirectionalShadowFrame>& f : frames) {
		f->input_texture_changed = true;
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::update()
{
	const unsigned int frame_number = e->get_frame_number();
	const std::shared_ptr<ForwardPbrDirectionalShadowFrame>& frame = frames[frame_number];
	if (frame->input_texture_changed) {
		frame->input_texture_changed = false;
		frame->pipeline_resource = std::static_pointer_cast<pipeline::ForwardPbrDirectionalShadowResource>(e->get_pipeline_manager()->create_resource(e->get_buffer_manager()->create_uniform(sizeof(ForwardPbrDirectionalShadowUniform), core::sync::EndCaller<core::sync::EndCallerIgnore>(0)), input_textures));
	}
	frame->primary_cmd->begin();
	for (const std::shared_ptr<ForwardPbrDirectionalShadowKernel>& kernel : frame->kernels) {
		kernel->latest_render_data_pool = 0;
		kernel->secondary_cmd->begin();
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::record(
	const std::shared_ptr<scene::Scene>& s,
	const std::shared_ptr<camera::Camera>& c,
	const std::shared_ptr<light::Directional>& l,
	const std::shared_ptr<model::Model>& m,
	const unsigned int kernel_index)
{
	const unsigned int frame_number = e->get_frame_number();
	const std::shared_ptr<ForwardPbrDirectionalShadowFrame>& frame = frames[frame_number];
	const std::shared_ptr<ForwardPbrDirectionalShadowKernel>& kernel = frame->kernels[kernel_index];
	const std::map<core::Id, std::shared_ptr<model::Mesh>>& meshes = m->get_meshes();
	for (const std::pair<core::Id, std::shared_ptr<model::Mesh>>& id_mesh : meshes) {
		const std::shared_ptr<mesh::Mesh>& msh = id_mesh.second->get_mesh();
		const std::shared_ptr<material::Material>& mat = id_mesh.second->get_material();
		if (kernel->latest_render_data_pool >= kernel->render_data_pool.size()) {
			kernel->render_data_pool.push_back(std::make_tuple(
				std::shared_ptr<buffer::Uniform>(e->get_buffer_manager()->create_uniform(sizeof(ForwardPbrDirectionalShadowUniform), e)),
				std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet>(static_cast<pipeline::ForwardPbrDirectionalShadowResourceSet*>(render_pipeline->create_resource_set()))));
		}
		std::tuple<std::shared_ptr<buffer::Uniform>, std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet>>& pool = kernel->render_data_pool[kernel->latest_render_data_pool];
		std::shared_ptr<buffer::Uniform>& ub = std::get<0>(pool);
		std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet>& prs = std::get<1>(pool);
		ForwardPbrDirectionalShadowUniform us;
		us.mvp = c->get_view_projection_matrix() * m->get_model_matrix();
		GXTODO;
		//us.light_view_projection_biases = l->get_view_projection_biases();
		us.light_color = math::Vec4(l->get_color(), 1.0);
		ub->update(&us);
		prs->set_scene(s);
		prs->set_camera(c);
		prs->set_light(l);
		prs->set_model(m);
		prs->set_mesh(msh);
		prs->set_material(mat);
		prs->set_effect(ub, frame->pipeline_resource);
		prs->record(kernel->secondary_cmd);
		++kernel->latest_render_data_pool;
	}
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::submit()
{
	const unsigned int frame_number = e->get_frame_number();
	const std::shared_ptr<ForwardPbrDirectionalShadowFrame>& frame = frames[frame_number];
	std::shared_ptr<command::Buffer>& cmd = frame->primary_cmd;
	render_target->bind(cmd);
	for (const std::shared_ptr<ForwardPbrDirectionalShadowKernel>& k : frame->kernels) {
		cmd->record(k->secondary_cmd);
	}
	std::vector<std::shared_ptr<sync::Semaphore>> pss(providers.size());
	for (const std::shared_ptr<core::graph::Node>& p : providers) {
		if (p == nullptr)
			continue;
		const std::shared_ptr<Node>& rp = std::static_pointer_cast<Node>(p);
		if (rp == nullptr)
			continue;
		const std::shared_ptr<sync::Semaphore>& ps = rp->get_semaphore(frame_number);
		if (ps == nullptr)
			continue;
		pss.push_back(ps);
	}
	e->submit(pss, cmd, frame->semaphore);
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadowFrame::ForwardPbrDirectionalShadowFrame(const std::shared_ptr<engine::Engine>& e)
	: primary_cmd(e->get_command_manager()->create_primary_command_buffer())
	, semaphore(e->create_semaphore())
{
	const unsigned int kernels_count = std::thread::hardware_concurrency();
	kernels.resize(kernels_count);
	for (unsigned int i = 0; i < kernels_count; ++i) {
		kernels[i] = std::make_shared<ForwardPbrDirectionalShadowKernel>(e, i);
	}
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadowKernel::ForwardPbrDirectionalShadowKernel(const std::shared_ptr<engine::Engine>& e, const unsigned int kernel_index)
	: secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}
