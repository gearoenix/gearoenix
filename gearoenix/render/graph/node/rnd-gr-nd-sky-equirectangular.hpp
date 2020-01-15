#ifndef GEAROENIX_RENDER_GRAPH_NODE_SKY_EQUIRECTANGULAR_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_SKY_EQUIRECTANGULAR_HPP
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
class SkyEquirectangularResourceSet;
}

namespace gearoenix::render::graph::node {

struct SkyEquirectangularUniform {
    math::Mat4x4 mvp;
};

struct SkyEquirectangularRenderData {
    std::unique_ptr<pipeline::SkyEquirectangularResourceSet> r;
    std::unique_ptr<buffer::Uniform> u;
    SkyEquirectangularRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
    ~SkyEquirectangularRenderData() noexcept;
};

struct SkyEquirectangularKernel {
    std::unique_ptr<command::Buffer> secondary_cmd;
    core::OneLoopPool<SkyEquirectangularRenderData> render_data_pool;
    SkyEquirectangularKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~SkyEquirectangularKernel() noexcept;
};

struct SkyEquirectangularFrame {
    std::vector<std::unique_ptr<SkyEquirectangularKernel>> kernels;
    explicit SkyEquirectangularFrame(engine::Engine* e) noexcept;
    ~SkyEquirectangularFrame() noexcept;
};

class SkyEquirectangular : public Node {
private:
    std::vector<std::unique_ptr<SkyEquirectangularFrame>> frames;
    SkyEquirectangularFrame* frame = nullptr;
    const camera::Camera* cam = nullptr;
    std::vector<const std::map<const model::Model*, std::vector<const model::Mesh*>>*> models;

    void record(const model::Mesh* msh, const SkyEquirectangularUniform& u, SkyEquirectangularKernel* kernel) noexcept;

public:
    SkyEquirectangular(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~SkyEquirectangular() noexcept final;
    void update() noexcept final;
    void set_camera(const camera::Camera* cam) noexcept;
    void add_models(const std::map<const model::Model*, std::vector<const model::Mesh*>>* models) noexcept;
    void record(unsigned int kernel_index) noexcept;
    void record_continuously(unsigned int kernel_index) noexcept;
    void submit() noexcept final;
};
}
#endif
