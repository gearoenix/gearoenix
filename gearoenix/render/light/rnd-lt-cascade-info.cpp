#include "rnd-lt-cascade-info.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../math/math-sphere.hpp"
#include "../../physics/accelerator/phs-acc-bvh.hpp"
#include "../../physics/collider/phs-cld-frustum.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../command/rnd-cmd-buffer.hpp"
#include "../command/rnd-cmd-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../graph/node/rnd-gr-nd-shadow-mapper.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../sync/rnd-sy-semaphore.hpp"
#include <limits>

gearoenix::render::light::CascadeInfo::PerCascade::PerCascade(engine::Engine* const e) noexcept
    : collider(new physics::collider::Frustum())
    , shadow_mapper(new graph::node::ShadowMapper(e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})))
{
}

gearoenix::render::light::CascadeInfo::PerCascade::~PerCascade() noexcept
{
    shadow_mapper = nullptr;
}

void gearoenix::render::light::CascadeInfo::PerKernel::shadow(const physics::collider::Collider* const cld, const std::size_t cascade_index) noexcept
{
    const model::Model* const m = cld->get_parent();
    if (!m->get_has_shadow_caster())
        return;
    // Be careful, the models should update the collider box
    math::Aabb3 box = cld->get_updated_box();
    box.set_center((*per_cascade)[0].collider->get_view_projection().project(box.get_center()));
    seen_boxes[cascade_index].put(box);
    RenderData r;
    r.i = cascade_index;
    r.m = m;
    render_data.push_back(r);
}

void gearoenix::render::light::CascadeInfo::PerKernel::record(const std::size_t kernel_index) noexcept
{
    for (auto& r : render_data) {
        auto& c = (*per_cascade)[r.i];
#ifdef GX_USE_OPENGL
#ifdef GX_USE_INSTEAD_OF_OPENGL
        if (GX_RUNTIME_USE_OPENGL) {
#endif
            c.shadow_mapper->record(c.view_projection_gl * r.m->get_collider()->get_model_matrix(), r.m, kernel_index);
#ifdef GX_USE_INSTEAD_OF_OPENGL
            continue;
        }
#endif
#endif
    }
}

void gearoenix::render::light::CascadeInfo::PerFrame::init(engine::Engine* const eng)
{
    auto* cmd_mgr = eng->get_command_manager();
    shadow_accumulator_secondary_command = std::unique_ptr<command::Buffer>(cmd_mgr->create_secondary_command_buffer());
    shadow_accumulator_primary_command = std::unique_ptr<command::Buffer>(cmd_mgr->create_primary_command_buffer());
}

void gearoenix::render::light::CascadeInfo::PerFrame::start() noexcept
{
    shadow_accumulator_secondary_command->begin();
    shadow_accumulator_primary_command->begin();
}

gearoenix::render::light::CascadeInfo::CascadeInfo(engine::Engine* const e) noexcept
    : e(e)
    , kernels(static_cast<std::size_t>(e->get_kernels()->get_threads_count()))
    , frames(static_cast<std::size_t>(e->get_frames_count()))
{
    const auto cascades_count = e->get_system_application()->get_configuration().render_config.shadow_cascades_count;
    for (auto& frame : frames)
        frame.init(e);
    for (auto i = 0; i < cascades_count; ++i)
        per_cascade.get_next([e] { return new PerCascade(e); });
    for (auto& k : kernels) {
        k.e = e;
        k.per_cascade = &per_cascade;
        k.seen_boxes.resize(cascades_count);
    }
}

gearoenix::render::light::CascadeInfo::~CascadeInfo() noexcept
{
    kernels.clear();
    frames.clear();
}

void gearoenix::render::light::CascadeInfo::update(const math::Mat4x4& m, const std::vector<std::array<math::Vec3, 4>>& p) noexcept
{
    current_frame = &frames[e->get_frame_number()];
    zero_located_view = m;

    const std::size_t s = p.size();
    const std::size_t ss = s - 1;
    const std::size_t sss = ss - 1;
    /// This provide the ability to have cameras with different cascaded shadow mapping partitions count
    /// And re-configurable render engine (in runtime cascaded-shadow-partitions-count can be changed.)
    if (ss != per_cascade.size()) {
        for (auto& k : kernels) {
            k.seen_boxes.resize(ss);
        }
    }
    per_cascade.refresh();
    for (std::size_t i = 0; i < ss; ++i) {
        auto* c = per_cascade.get_next([this] { return new PerCascade(e); });
        c->intersection_box.reset();
        c->collider->get_limit().reset();
        c->collider->set_view_projection(m);
        c->max_box.reset();
    }
    for (auto& k : kernels) {
        auto& seen_boxes = k.seen_boxes;
        for (auto& seen : seen_boxes) {
            seen.reset();
        }
        k.render_data.clear();
    }
    for (const auto& v : p[0]) {
        per_cascade[0].collider->get_limit().put_without_update(zero_located_view.project(v));
    }
    for (std::size_t i = 1, j = 0; i < ss; ++i, ++j) {
        for (const auto& v : p[i]) {
            const auto vv = zero_located_view.project(v);
            per_cascade[i].collider->get_limit().put_without_update(vv);
            per_cascade[j].collider->get_limit().put_without_update(vv);
        }
    }
    for (const auto& v : p[ss]) {
        per_cascade[sss].collider->get_limit().put_without_update(zero_located_view.project(v));
    }
    for (auto& c : per_cascade) {
        auto& limit = c.collider->get_limit();
        math::Vec3 v = limit.get_upper();
        GXTODO // some doubt about it
            v[2]
            = std::numeric_limits<core::Real>::max();
        limit.put_without_update(v);
        limit.update();
    }
}

void gearoenix::render::light::CascadeInfo::start() noexcept
{
    current_frame->start();
}

void gearoenix::render::light::CascadeInfo::shadow(const physics::accelerator::Bvh* bvh, const std::size_t kernel_index) noexcept
{
    const std::size_t cascades_count = per_cascade.size();
    for (std::size_t i = 0; i < cascades_count; ++i) {
        bvh->call_on_intersecting(per_cascade[i].collider.get(), [this, i, kernel_index](physics::collider::Collider* cld) {
            kernels[kernel_index].shadow(cld, i);
        });
    }
}

void gearoenix::render::light::CascadeInfo::shrink() noexcept
{
    const auto cc = per_cascade.size();
    for (auto k : kernels) {
        auto& seen = k.seen_boxes;
        for (std::size_t i = 0; i < cc; ++i) {
            per_cascade[i].max_box.put(seen[i]);
        }
    }
    for (auto& cas : per_cascade) {
        cas.shadow_mapper->update();
        (void)cas.max_box.check_intersection(cas.collider->get_limit(), cas.intersection_box);
        const auto& mx = cas.intersection_box.get_upper();
        const auto& mn = cas.intersection_box.get_lower();
        const auto c = (mx + mn) * 0.5f;
        const auto d = mx - mn;
        const auto w = d[0] * 1.01f;
        const auto h = d[1] * 1.01f;
        const auto depth = d[2];
        const auto n = depth * 0.01f;
        const auto f = depth * 1.03f;
        const auto p = gearoenix::math::Mat4x4::orthographic(w, h, n, f);
        const auto t = gearoenix::math::Mat4x4::translator(-gearoenix::math::Vec3(c.xy(), mx[2] + (n * 2.0f)));
        const auto mtx = p * t * zero_located_view;

#ifdef GX_USE_OPENGL
#ifdef GX_USE_INSTEAD_OF_OPENGL
        if (GX_RUNTIME_USE_OPENGL) {
#endif
            cas.view_projection_gl = mtx;
            cas.view_projection_bias_gl = math::Mat4x4(
                                              0.5f, 0.0f, 0.0f, 0.0f,
                                              0.0f, 0.5f, 0.0f, 0.0f,
                                              0.0f, 0.0f, 1.0f, 0.0f,
                                              0.5f, 0.5f, 0.0f, 1.0f)
                * mtx;

#ifdef GX_USE_INSTEAD_OF_OPENGL
        }
#endif
#endif
    }
}

void gearoenix::render::light::CascadeInfo::record(std::size_t kernel_index) noexcept
{
    kernels[kernel_index].record(kernel_index);
}

void gearoenix::render::light::CascadeInfo::submit() noexcept
{
    for (auto& c : per_cascade)
        c.shadow_mapper->submit();
}

const gearoenix::core::OneLoopPool<gearoenix::render::light::CascadeInfo::PerCascade>& gearoenix::render::light::CascadeInfo::get_cascades_data() const noexcept
{
    return per_cascade;
}

gearoenix::core::OneLoopPool<gearoenix::render::light::CascadeInfo::PerCascade>& gearoenix::render::light::CascadeInfo::get_cascades_data() noexcept
{
    return per_cascade;
}
