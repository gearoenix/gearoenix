#include "rnd-lt-cascade-info.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../math/math-sphere.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-configuration.hpp"
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
    : shadow_mapper(new graph::node::ShadowMapper(e, core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})))
{
}

void gearoenix::render::light::CascadeInfo::PerKernel::shadow(const model::Model* const m) noexcept
{
    const math::Sphere& ms = m->get_occlusion_sphere();
    const math::Sphere s((*zero_located_view) * ms.position, ms.radius);
    const std::size_t cascades_count = seen_boxes.size();
    for (std::size_t i = 0; i < cascades_count; ++i) {
        if ((*per_cascade)[i].limit_box.test(s)) {
            seen_boxes[i].put(s);
            gearoenix::render::light::CascadeInfo::RenderData r;
            r.i = i;
            r.m = m;
            render_data.push_back(r);
        }
    }
}

void gearoenix::render::light::CascadeInfo::PerKernel::record(const std::size_t kernel_index) noexcept
{
    for (auto& r : render_data) {
        auto& c = (*per_cascade)[r.i];
#ifdef GX_USE_OPENGL
#ifdef GX_USE_INSTEAD_OF_OPENGL
        if (GX_RUNTIME_USE_OPENGL) {
#endif
            c.shadow_mapper->record(c.view_projection_gl * r.m->get_model_matrix(), r.m, kernel_index);
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
    shadow_accumulator_semaphore = std::unique_ptr<sync::Semaphore>(eng->create_semaphore());
}

void gearoenix::render::light::CascadeInfo::PerFrame::start() noexcept
{
    /// TODO: these must move to new node structure for shadow accumulation
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
        k.zero_located_view = &zero_located_view;
    }
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
        c->limit_box.reset();
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
    for (auto& c : per_cascade) {
        gearoenix::math::Vec3 v = c.limit_box.mx;
        v[2] = -std::numeric_limits<gearoenix::core::Real>::max();
        c.limit_box.put(v);
    }
}

void gearoenix::render::light::CascadeInfo::start() noexcept
{
    current_frame->start();
}

void gearoenix::render::light::CascadeInfo::shadow(const model::Model* m, const std::size_t kernel_index) noexcept
{
    kernels[kernel_index].shadow(m);
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
        cas.max_box.test(cas.limit_box, cas.intersection_box);
        const auto& mx = cas.intersection_box.mx;
        const auto& mn = cas.intersection_box.mn;
        const auto c = (mx + mn) * 0.5f;
        const auto d = mx - mn;
        const auto w = d[0] * 1.01f;
        const auto h = d[1] * 1.01f;
        const auto depth = d[2];
        const auto n = depth * 0.01;
        const auto f = depth * 1.03;
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
