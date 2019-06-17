#include "rnd-lt-cascade-info.hpp"
#include "../../core/cr-pool.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-sphere.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-configuration.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../command/rnd-cmd-buffer.hpp"
#include "../command/rnd-cmd-manager.hpp"
#include "../graph/node/rnd-gr-nd-shadow-mapper.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../sync/rnd-sy-semaphore.hpp"
#include <limits>

struct FramePerCascadeInfo {
	gearoenix::math::Mat4x4 view_projection;
	gearoenix::math::Aabb3 limit_box;
	gearoenix::math::Aabb3 max_box;
	gearoenix::math::Aabb3 intersection_box;
};

struct KernelRenderCascadeInfo {
    std::size_t cascade_index = static_cast<std::size_t>(-1);
    /// It is not owner of model
    const gearoenix::render::model::Model* m = nullptr;
};

struct KernelCascadeInfo {
	const gearoenix::math::Mat4x4 *zero_located_view;
	const std::vector<FramePerCascadeInfo>* per_cascade = nullptr;
	/// Per cascade
	std::vector<gearoenix::math::Aabb3> seen_boxes;
    std::vector<KernelRenderCascadeInfo> render_data_pool;

    void shadow(const gearoenix::render::model::Model* m) noexcept
    {
        const gearoenix::math::Sphere& ms = m->get_occlusion_sphere();
        const gearoenix::math::Sphere s((*zero_located_view) * ms.position, ms.radius);
        const std::size_t cascades_count = seen_boxes.size();
        for (std::size_t i = 0; i < cascades_count; ++i) {
            if ((*per_cascade)[i].limit_box.test(s)) {
                seen_boxes[i].put(s);
				KernelRenderCascadeInfo r;
                r.cascade_index = i;
                r.m = m;
				render_data_pool.push_back(r);
            }
        }
    }
};

struct FrameCascadeInfo {
    gearoenix::render::engine::Engine* const e;
    gearoenix::math::Mat4x4 zero_located_view;
    std::vector<KernelCascadeInfo> kernels;
    std::vector<FramePerCascadeInfo> per_cascade;
    /// Accumulate shadow
    /// TODO: place then in a new node structure
    gearoenix::render::command::Buffer* shadow_accumulator_secondary_command = nullptr;
    gearoenix::render::command::Buffer* shadow_accumulator_primary_command = nullptr;
    gearoenix::render::sync::Semaphore* shadow_accumulator_semaphore = nullptr;

    explicit FrameCascadeInfo(gearoenix::render::engine::Engine* e) noexcept
        : kernels(static_cast<std::size_t>(e->get_kernels()->get_threads_count()))
        , e(e)
        , shadow_accumulator_secondary_command(e->get_command_manager()->create_secondary_command_buffer())
        , shadow_accumulator_primary_command(e->get_command_manager()->create_primary_command_buffer())
        , shadow_accumulator_semaphore(e->create_semaphore())
    {
        for (auto &k: kernels) {
			k.per_cascade = &per_cascade;
			k.zero_located_view = &zero_located_view;
        }
    }

    ~FrameCascadeInfo() noexcept
    {
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
        const std::size_t s = p.size();
        const std::size_t ss = s - 1;
        const std::size_t sss = ss - 1;
        /// This provide the ability to have cameras with different cascaded shadow mapping partitions count
        /// And re-configurable render engine (in runtime cascaded-shadow-partitions-count can be changed.)
		if (ss != per_cascade.size()) 
		{
			per_cascade.resize(ss);
			for (auto& k : kernels)
			{
				k.seen_boxes.resize(ss);
			}
		}
		for (auto& c : per_cascade) 
		{
			c.intersection_box.reset();
			c.limit_box.reset();
			c.max_box.reset();
		}
		for (auto& k : kernels)
		{
			auto &seen_boxes = k.seen_boxes;
			for (auto& s : seen_boxes)
			{
				s.reset();
			}
			k.render_data_pool.clear();
		}
        gearoenix::render::command::Manager* cmdmgr = e->get_command_manager();
        for (const auto& v : p[0]) {
            per_cascade[0].limit_box.put(zero_located_view * v);
        }
        for (std::size_t i = 1, j = 0; i < ss; ++i, ++j) {
            for (const auto& v : p[i]) {
                const auto vv = zero_located_view * v;
				per_cascade[i].limit_box.put(vv);
				per_cascade[j].limit_box.put(vv);
            }
        }
        for (const auto& v : p[ss]) {
			per_cascade[sss].limit_box.put(zero_located_view * v);
        }
        for (auto& p : per_cascade) {
            gearoenix::math::Vec3 v = p.limit_box.mx;
            v[2] = -std::numeric_limits<gearoenix::core::Real>::max();
			p.limit_box.put(v);
        }
    }

    void start() noexcept
    {
		/// TODO: these must move to new node structure for shadow accumulation
        shadow_accumulator_secondary_command->begin();
        shadow_accumulator_primary_command->begin();
    }

    void shadow(const gearoenix::render::model::Model* m, const std::size_t kernel_index) noexcept
    {
        kernels[kernel_index].shadow(m);
    }

    void shrink() noexcept
    {
        const auto cc = per_cascade.size();
        for (auto k : kernels) {
            auto& seen = k.seen_boxes;
            for (std::size_t i = 0; i < cc; ++i) {
                per_cascade[i].max_box.put(seen[i]);
            }
        }
        for (auto &cas: per_cascade) {
			cas.max_box.test(cas.limit_box, cas.intersection_box);
            const auto& mx = cas.intersection_box.mx;
            const auto& mn = cas.intersection_box.mn;
            const auto c = (mx + mn) * 0.5f;
            const auto d = mx - mn;
            const auto w = d[0] * 0.51f;
            const auto h = d[1] * 0.51f;
            const auto depth = d[2];
            const auto n = depth * 0.01;
            const auto f = depth * 1.03;
            const auto p = gearoenix::math::Mat4x4::orthographic(w, h, n, f);
            const auto t = gearoenix::math::Mat4x4::translator(-gearoenix::math::Vec3(c.xy(), mx[2] + (n * 2.0f)));
            cas.view_projection = p * t * zero_located_view;
        }
    }
};

gearoenix::render::light::CascadeInfo::CascadeInfo(engine::Engine* const e) noexcept
    : e(e)
    , frames(static_cast<std::size_t>(e->get_frames_count()))
{
	const core::sync::EndCaller<core::sync::EndCallerIgnore> call([] {});
	const auto cascades_count = e->get_system_application()->get_configuration().render_config.shadow_cascades_count;
    for (FrameCascadeInfo*& frame : frames)
        frame = new FrameCascadeInfo(e);
	for (auto i = 0; i < cascades_count; ++i)
		shadow_mappers.get_next([e, call] {
			return new graph::node::ShadowMapper(e, call);
		});
}

gearoenix::render::light::CascadeInfo::~CascadeInfo() noexcept
{
    for (FrameCascadeInfo* frame : frames)
        delete frame;
    frames.clear();
}

void gearoenix::render::light::CascadeInfo::update(const math::Mat4x4& m, const std::vector<std::array<math::Vec3, 4>>& p) noexcept
{
    current_frame = frames[e->get_frame_number()];
    current_frame->set_zero_located_view(m);
    current_frame->set_frustum_partitions(p);
}

void gearoenix::render::light::CascadeInfo::start() noexcept
{
	const auto cc = current_frame->per_cascade.size();
	shadow_mappers.refresh();
	for(auto i = 0; i < cc; ++i)
		shadow_mappers.get_next([this] { return new graph::node::ShadowMapper(e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})); })->update();
    current_frame->start();
}

void gearoenix::render::light::CascadeInfo::shadow(const model::Model* m, const std::size_t kernel_index) noexcept
{
    current_frame->shadow(m, kernel_index);
}

void gearoenix::render::light::CascadeInfo::shrink() noexcept
{
    current_frame->shrink();
}

void gearoenix::render::light::CascadeInfo::record(std::size_t kernel_index) noexcept
{

}

void gearoenix::render::light::CascadeInfo::submit() noexcept
{
}
