#include "rnd-lt-cascade-info.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-matrix.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-configuration.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../command/rnd-cmd-buffer.hpp"
#include "../command/rnd-cmd-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../sync/rnd-sy-semaphore.hpp"

static struct CameraCascadeInfo {
	gearoenix::math::Mat4x4 vp;
	gearoenix::math::Aabb3 boundary;
	gearoenix::math::Aabb3 seen;
};

static struct MathCascadeInfo {
	gearoenix::math::Mat4x4 zero_located_view;
	std::vector<CameraCascadeInfo> cameras;

	explicit MathCascadeInfo(gearoenix::render::engine::Engine* e) noexcept
		: cameras(e->get_system_application()->get_configuration().render_config.shadow_cascades_count) {}
	
	void initialize(gearoenix::render::engine::Engine* e) noexcept
	{
		cameras.resize(e->get_system_application()->get_configuration().render_config.shadow_cascades_count);
	}
};

static struct KernelRenderCascadeInfo {
	gearoenix::render::buffer::Uniform* uniform = nullptr;
	std::size_t cascade_index = 0;
	/// It is not owner of model
	gearoenix::render::model::Model* m = nullptr;

	~KernelRenderCascadeInfo() noexcept
	{
		delete uniform;
		uniform = nullptr;
	}
};

static struct KernelCascadeInfo {
	const std::size_t kernel_index;
	MathCascadeInfo math_info;
	std::vector<gearoenix::render::command::Buffer*> shadow_mapper_secondary_commands;
	/// Pool data
	std::vector<KernelRenderCascadeInfo> render_data_pool;
	std::size_t pool_last_index = 0;

	KernelCascadeInfo(gearoenix::render::engine::Engine* e, std::size_t kernel_index) noexcept
		: kernel_index(kernel_index)
		, math_info(e)
		, shadow_mapper_secondary_commands(static_cast<std::size_t>(e->get_system_application()->get_configuration().render_config.shadow_cascades_count))
	{
		gearoenix::render::command::Manager* cmdmgr = e->get_command_manager();
		for (gearoenix::render::command::Buffer*& b : shadow_mapper_secondary_commands)
			b = cmdmgr->create_secondary_command_buffer(kernel_index);
	}

	~KernelCascadeInfo() noexcept
	{
		for (gearoenix::render::command::Buffer* b : shadow_mapper_secondary_commands)
			delete b;
		shadow_mapper_secondary_commands.clear();
		render_data_pool.clear();
		pool_last_index = 0;
	}

	void set_zero_located_view(const gearoenix::math::Mat4x4& m) noexcept
	{
		math_info.zero_located_view = m;
	}

	void set_frustum_partitions(const std::vector<std::array<gearoenix::math::Vec3, 4> >& p) noexcept;
};

static struct FrameCascadeInfo {
	std::vector<KernelCascadeInfo*> kernels;
	/// For each cascades
	std::vector<gearoenix::render::command::Buffer*> shadow_mapper_primary_commands;
	gearoenix::render::sync::Semaphore* shadow_mappers_semaphore = nullptr;
	/// Accumulate shadow
	gearoenix::render::command::Buffer* shadow_accumulator_secondary_command = nullptr;
	gearoenix::render::command::Buffer* shadow_accumulator_primary_command = nullptr;
	gearoenix::render::sync::Semaphore* shadow_accumulator_semaphore = nullptr;

	explicit FrameCascadeInfo(gearoenix::render::engine::Engine* e) noexcept
		: kernels(static_cast<std::size_t>(e->get_kernels()->get_threads_count()))
		, shadow_mapper_primary_commands(static_cast<std::size_t>(e->get_system_application()->get_configuration().render_config.shadow_cascades_count))
		, shadow_mappers_semaphore(e->create_semaphore())
		, shadow_accumulator_secondary_command(e->get_command_manager()->create_secondary_command_buffer())
		, shadow_accumulator_primary_command(e->get_command_manager()->create_primary_command_buffer())
		, shadow_accumulator_semaphore(e->create_semaphore())
	{
		for (std::size_t i = 0; i < kernels.size(); ++i)
			kernels[i] = new KernelCascadeInfo(e, i);
		gearoenix::render::command::Manager * cmdmgr = e->get_command_manager();
		for (gearoenix::render::command::Buffer*& b : shadow_mapper_primary_commands)
			b = cmdmgr->create_primary_command_buffer();
	}

	~FrameCascadeInfo() noexcept
	{
		for (KernelCascadeInfo* k : kernels)
			delete k;
		kernels.clear();
		for (gearoenix::render::command::Buffer* b : shadow_mapper_primary_commands)
			delete b;
		shadow_mapper_primary_commands.clear();
		delete shadow_mappers_semaphore;
		shadow_mappers_semaphore = nullptr;
		delete shadow_accumulator_secondary_command;
		shadow_accumulator_secondary_command = nullptr;
		delete shadow_accumulator_primary_command;
		shadow_accumulator_primary_command = nullptr;
		delete shadow_accumulator_semaphore;
		shadow_accumulator_semaphore = nullptr;
	}

	void set_zero_located_view(const gearoenix::math::Mat4x4& m) noexcept
	{
		for (KernelCascadeInfo* k : kernels) k->set_zero_located_view(m);
	}

	void set_frustum_partitions(const std::vector<std::array<gearoenix::math::Vec3, 4> >& p) noexcept
	{
		const auto& m = kernels[0]->math_info.zero_located_view;
		const std::size_t s = p.size();
		const std::size_t ss = s - 1;
		std::vector<gearoenix::math::Aabb3> b(ss);
		for(std::size_t i = 0; i < 4; ++i) 
		for(std::size_t i = 0; i < s; ++i)


			for (KernelCascadeInfo* k : kernels) k->set_frustum_partitions(p);
	}
};

gearoenix::render::light::CascadeInfo::CascadeInfo(engine::Engine* const e) noexcept
    : e(e)
	, frames(static_cast<std::size_t>(e->get_frames_count()))
{
    for (FrameCascadeInfo*& frame : frames)
        frame = new FrameCascadeInfo(e);
}

gearoenix::render::light::CascadeInfo::~CascadeInfo() noexcept
{
    for (FrameCascadeInfo* frame : frames)
        delete frame;
    frames.clear();
}

void gearoenix::render::light::CascadeInfo::set_zero_located_view(const math::Mat4x4& m) noexcept
{
	frames[e->get_frame_number()]->set_zero_located_view(m);
}

void gearoenix::render::light::CascadeInfo::set_frustum_partitions(const std::vector<std::array<math::Vec3, 4>>& p) noexcept
{
	frames[e->get_frame_number()]->set_frustum_partitions(p);
}
