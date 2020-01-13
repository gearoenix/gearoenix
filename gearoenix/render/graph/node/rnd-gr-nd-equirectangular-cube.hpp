#ifndef GEAROENIX_RENDER_GRAPH_NODE_EQUIRECTANGULAR_CUBE_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_EQUIRECTANGULAR_CUBE_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix.hpp"
#include "../../../math/math-vector.hpp"
#include "../../material/rnd-mat-translucency-mode.hpp"
#include "rnd-gr-nd-node.hpp"
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
class EquirectangularCubeResourceSet;
}

namespace gearoenix::render::graph::node {

struct EquirectangularCubeUniform {
    math::Mat4x4 mvp;
};

struct EquirectangularCubeRenderData {
    std::unique_ptr<pipeline::EquirectangularCubeResourceSet> r;
    std::unique_ptr<buffer::Uniform> u;
    EquirectangularCubeRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
    ~EquirectangularCubeRenderData() noexcept;
};

struct EquirectangularCubeKernel {
    std::unique_ptr<command::Buffer> secondary_cmd;
    core::OneLoopPool<EquirectangularCubeRenderData> render_data_pool;
    EquirectangularCubeKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~EquirectangularCubeKernel() noexcept;
};

struct EquirectangularCubeFrame {
    std::vector<std::unique_ptr<EquirectangularCubeKernel>> kernels;
    explicit EquirectangularCubeFrame(engine::Engine* e) noexcept;
    ~EquirectangularCubeFrame() noexcept;
};

class EquirectangularCube : public Node {
private:
    std::vector<std::unique_ptr<EquirectangularCubeFrame>> frames;
    EquirectangularCubeFrame* frame = nullptr;
    const camera::Camera* cam = nullptr;
    std::vector<const std::map<const model::Model*, std::vector<const model::Mesh*>>*> models;

    void record(const model::Mesh* msh, const EquirectangularCubeUniform& u, EquirectangularCubeKernel* kernel) noexcept;

public:
    EquirectangularCube(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~EquirectangularCube() noexcept final;
    void update() noexcept final;
    void set_camera(const camera::Camera* cam) noexcept;
    void add_models(const std::map<const model::Model*, std::vector<const model::Mesh*>>* models) noexcept;
    void record(unsigned int kernel_index) noexcept;
    void record_continuously(unsigned int kernel_index) noexcept;
    void submit() noexcept final;
};
}
#endif
