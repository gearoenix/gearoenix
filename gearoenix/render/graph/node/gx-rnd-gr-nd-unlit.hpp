#ifndef GEAROENIX_RENDER_GRAPH_NODE_UNLIT_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_UNLIT_HPP
#include "../../../core/gx-cr-pool.hpp"
#include "../../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../../math/gx-math-matrix-4d.hpp"
#include "../../material/gx-rnd-mat-translucency-mode.hpp"
#include "gx-rnd-gr-nd-node.hpp"
#include <vector>

namespace gearoenix::render::buffer {
class Uniform;
}

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::command {
class Buffer;
}

namespace gearoenix::render::model {
class Model;
class Mesh;
}

namespace gearoenix::render::pipeline {
class UnlitResourceSet;
}

namespace gearoenix::render::graph::node {

struct UnlitUniform {
    math::Mat4x4<float> mvp;
};

struct UnlitRenderData {
    std::unique_ptr<pipeline::UnlitResourceSet> r;
    std::shared_ptr<buffer::Uniform> u;
    UnlitRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
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

class Unlit final : public Node {
private:
    std::vector<std::unique_ptr<UnlitFrame>> frames;
    UnlitFrame* frame = nullptr;
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
