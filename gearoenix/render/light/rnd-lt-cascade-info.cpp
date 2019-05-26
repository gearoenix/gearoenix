#include "rnd-lt-cascade-info.hpp"
#include "../../core/cr-pool.hpp"
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

struct MathCascadeInfo {
    const gearoenix::math::Mat4x4* zero_located_view = nullptr;
    const gearoenix::core::OneLoopPool<gearoenix::math::Aabb3>* limit_boxes = nullptr;
	gearoenix::core::OneLoopPool<gearoenix::math::Aabb3> seen_boxes;
};

struct KernelRenderCascadeInfo {
    gearoenix::render::buffer::Uniform* uniform = nullptr;
    std::size_t cascade_index = 0;
    /// It is not owner of model
    const gearoenix::render::model::Model* m = nullptr;

    ~KernelRenderCascadeInfo() noexcept
    {
        delete uniform;
        uniform = nullptr;
		cascade_index = static_cast<std::size_t>(-1);
		m = nullptr;
    }
};

struct KernelCascadeInfo {
    const std::size_t kernel_index;
    MathCascadeInfo math_info;
	gearoenix::core::OneLoopPool<gearoenix::render::command::Buffer> shadow_mapper_secondary_commands;
    gearoenix::core::OneLoopPool<KernelRenderCascadeInfo> render_data_pool;

    KernelCascadeInfo(std::size_t kernel_index) noexcept : kernel_index(kernel_index) {}
};

struct FrameCascadeInfo {
    gearoenix::math::Mat4x4 zero_located_view;
    std::vector<KernelCascadeInfo*> kernels;
	gearoenix::render::engine::Engine* const e;
    /// For each cascades
	gearoenix::core::OneLoopPool<gearoenix::math::Aabb3> limit_boxes;
	gearoenix::core::OneLoopPool<gearoenix::render::command::Buffer> shadow_mapper_primary_commands;
    gearoenix::render::sync::Semaphore* shadow_mappers_semaphore = nullptr;
    /// Accumulate shadow
    gearoenix::render::command::Buffer* shadow_accumulator_secondary_command = nullptr;
    gearoenix::render::command::Buffer* shadow_accumulator_primary_command = nullptr;
    gearoenix::render::sync::Semaphore* shadow_accumulator_semaphore = nullptr;

    explicit FrameCascadeInfo(gearoenix::render::engine::Engine* e) noexcept
        : kernels(static_cast<std::size_t>(e->get_kernels()->get_threads_count()))
		, e(e)
        , shadow_mappers_semaphore(e->create_semaphore())
        , shadow_accumulator_secondary_command(e->get_command_manager()->create_secondary_command_buffer())
        , shadow_accumulator_primary_command(e->get_command_manager()->create_primary_command_buffer())
        , shadow_accumulator_semaphore(e->create_semaphore())
    {
        for (std::size_t i = 0; i < kernels.size(); ++i) {
            auto k = new KernelCascadeInfo(i);
            kernels[i] = k;
            k->math_info.limit_boxes = &limit_boxes;
            k->math_info.zero_located_view = &zero_located_view;
        }
    }

    ~FrameCascadeInfo() noexcept
    {
        for (KernelCascadeInfo* k : kernels)
            delete k;
        kernels.clear();
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
        zero_located_view = m;
    }

    void set_frustum_partitions(const std::vector<std::array<gearoenix::math::Vec3, 4>>& p) noexcept
    {
        const auto& m = *(kernels[0]->math_info.zero_located_view);
        const std::size_t s = p.size();
        const std::size_t ss = s - 1;
        const std::size_t sss = ss - 1;
        /// This provide the ability to have cameras with different cascaded shadow mapping partitions count
		/// And re-configurable render engine (in runtime cascaded-shadow-partitions-count can be changed.)
        limit_boxes.refresh();
		shadow_mapper_primary_commands.refresh();
		for (auto* k : kernels) {
			k->render_data_pool.refresh();
			k->shadow_mapper_secondary_commands.refresh();
			k->math_info.seen_boxes.refresh();
		}
		gearoenix::render::command::Manager* cmdmgr = e->get_command_manager();
		for (std::size_t i = 0; i < ss; ++i) {
			limit_boxes.get_next([] { return new gearoenix::math::Aabb3; })->reset();
			shadow_mapper_primary_commands.get_next([cmdmgr] { return cmdmgr->create_primary_command_buffer(); });
			for (auto* k : kernels) {
				k->shadow_mapper_secondary_commands.get_next([cmdmgr, k] { 
					return cmdmgr->create_secondary_command_buffer(k->kernel_index); });
				k->math_info.seen_boxes.get_next([] { 
					return new gearoenix::math::Aabb3; })->reset();
			}
		}
        for (const auto& v : p[0]) {
            limit_boxes[0].put(m * v);
        }
        for (std::size_t i = 1, j = 0; i < ss; ++i, ++j) {
            for (const auto& v : p[i]) {
                const auto vv = m * v;
                limit_boxes[i].put(vv);
                limit_boxes[j].put(vv);
            }
        }
        for (const auto& v : p[ss]) {
            limit_boxes[sss].put(m * v);
        }
    }

	void start() noexcept {
		for (auto* k : kernels) for (auto& c : k->shadow_mapper_secondary_commands) c.begin();
		for (auto& c : shadow_mapper_primary_commands) c.begin();
		shadow_accumulator_secondary_command->begin();
		shadow_accumulator_primary_command->begin();
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

void gearoenix::render::light::CascadeInfo::update(const math::Mat4x4& m, const std::vector<std::array<math::Vec3, 4>>& p) noexcept
{
	FrameCascadeInfo* frame = frames[e->get_frame_number()];
    frame->set_zero_located_view(m);
    frame->set_frustum_partitions(p);
}

void gearoenix::render::light::CascadeInfo::start() noexcept
{
	frames[e->get_frame_number()]->start();
}
