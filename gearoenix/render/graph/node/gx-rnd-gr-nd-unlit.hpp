#ifndef GEAROENIX_RENDER_GRAPH_NODE_UNLIT_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_UNLIT_HPP
#include "../../../core/gx-cr-pool.hpp"
#include "../../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../../math/gx-math-matrix-4d.hpp"
#include "gearoenix/render/gx-rnd-translucency-mode.hpp"
#include "gx-rnd-gr-nd-node.hpp"
#include <limits>
#include <vector>

namespace gearoenix::render::buffer {
struct Uniform;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::command {
struct Buffer;
}

namespace gearoenix::render::model {
struct Model;
struct Mesh;
}

namespace gearoenix::render::pipeline {
struct UnlitResourceSet;
}

namespace gearoenix::render::graph::node {

struct UnlitUniform {
    math::Mat4x4<float> mvp;
};

struct UnlitRenderData {
    std::unique_ptr<pipeline::UnlitResourceSet> r;
    std::shared_ptr<buffer::Uniform> u;
    UnlitRenderData(engine::Engine* e, pipeline::Pipeline* pip, std::size_t frame_number) noexcept;
    ~UnlitRenderData() noexcept;
};

struct UnlitKernel {
    std::unique_ptr<command::Buffer> secondary_cmd;
    core::OneLoopPool<UnlitRenderData> render_data_pool;
    UnlitKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~UnlitKernel() noexcept;
};

struct UnlitFrame {
    std::vector<std::unique_ptr<UnlitKernel>> kernels;
    explicit UnlitFrame(engine::Engine* e) noexcept;
    ~UnlitFrame() noexcept;
};

struct Unlit final : public Node {
private:
    std::vector<std::unique_ptr<UnlitFrame>> frames;
    UnlitFrame* frame = nullptr;
    std::size_t frame_number = std::numeric_limits<std::size_t>::max();
    const camera::Camera* cam = nullptr;
    std::vector<std::pair<model::Model*, model::Mesh*>> meshes;

    void record(const model::Mesh* msh, const UnlitUniform& u, UnlitKernel* kernel) noexcept;

public:
    Unlit(
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~Unlit() noexcept final;
    void update() noexcept final;
    void set_camera(const camera::Camera* cam) noexcept;
    void add_mesh(std::pair<model::Model*, model::Mesh*> m) noexcept;
    void record(unsigned int kernel_index) noexcept final;
    void record_continuously(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif
