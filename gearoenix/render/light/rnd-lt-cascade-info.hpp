#ifndef GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#define GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-pool.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-matrix-4d.hpp"
#include <array>
#include <memory>
#include <vector>

namespace gearoenix::physics::accelerator {
struct Bvh;
}

namespace gearoenix::physics::collider {
class Collider;
class Transform;
}

namespace gearoenix::render::command {
class Buffer;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::graph::node {
class ShadowMapper;
}

namespace gearoenix::render::sync {
class Semaphore;
}

namespace gearoenix::render::light {
class Directional;
/// For shadow rendering following function happen in order:
///     - update (in kernels)
///     - start  (in main)    [because it begins command buffers]
///     - shadow (in kernels)
///     - shrink (in main)
///     - record (in kernels)
///     - submit (in main)
class CascadeInfo {
    GX_GETSET_PTRC_PRV(Directional, source)
public:
    struct PerCascade {
#ifdef GX_USE_OPENGL
        math::Mat4x4<double> view_projection_gl;
        math::Mat4x4<double> view_projection_bias_gl;
#endif
        std::unique_ptr<physics::collider::Transform> collider;
        math::Aabb3 max_box;
        math::Aabb3 intersection_box;
        std::unique_ptr<graph::node::ShadowMapper> shadow_mapper;

        explicit PerCascade(engine::Engine* e) noexcept;
        ~PerCascade() noexcept;
    };

private:
    struct RenderData {
        /// Cascade index
        std::size_t i = static_cast<std::size_t>(-1);
        /// It is not owner of model
        const model::Model* m = nullptr;
    };
    struct PerKernel {
        engine::Engine* e = nullptr;
        const core::OneLoopPool<PerCascade>* per_cascade = nullptr;
        std::vector<RenderData> render_data;
        /// Per cascade
        std::vector<math::Aabb3> seen_boxes;
        void shadow(const physics::collider::Collider* cld, std::size_t cascade_index) noexcept;
        void record(std::size_t kernel_index) noexcept;
    };
    struct PerFrame {
        std::unique_ptr<command::Buffer> shadow_accumulator_secondary_command;
        std::unique_ptr<command::Buffer> shadow_accumulator_primary_command;

        void init(engine::Engine* e);
        void start() noexcept;
    };
    engine::Engine* const e;

    math::Mat4x4<double> zero_located_view;
    std::vector<PerKernel> kernels;
    std::vector<PerFrame> frames;
    /// Per cascade
    core::OneLoopPool<PerCascade> per_cascade;
    PerFrame* current_frame = nullptr;

public:
    explicit CascadeInfo(engine::Engine* e) noexcept;
    ~CascadeInfo() noexcept;

    void update(const math::Mat4x4<double>& m, const std::vector<std::array<math::Vec3<double>, 4>>& p) noexcept;

    void start() noexcept;

    void shadow(const physics::accelerator::Bvh* bvh, std::size_t kernel_index) noexcept;

    void shrink() noexcept;

    void record(std::size_t kernel_index) noexcept;

    void submit() noexcept;

    [[nodiscard]] const core::OneLoopPool<PerCascade>& get_cascades_data() const noexcept;
    [[nodiscard]] core::OneLoopPool<PerCascade>& get_cascades_data() noexcept;
};
}
#endif