#ifndef GEAROENIX_RENDER_GRAPH_NODE_IRRADIANCE_CONVOLUTER_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_IRRADIANCE_CONVOLUTER_HPP
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

namespace gearoenix::render::mesh {
class Mesh;
}

namespace gearoenix::render::pipeline {
class IrradianceConvoluterResourceSet;
}

namespace gearoenix::render::graph::node {

struct IrradianceConvoluterRenderData {
    std::unique_ptr<pipeline::IrradianceConvoluterResourceSet> r;
    IrradianceConvoluterRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
    ~IrradianceConvoluterRenderData() noexcept;
};

struct IrradianceConvoluterKernel {
    std::unique_ptr<command::Buffer> secondary_cmd;
    core::OneLoopPool<IrradianceConvoluterRenderData> render_data_pool;
    IrradianceConvoluterKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~IrradianceConvoluterKernel() noexcept;
};

struct IrradianceConvoluterFrame {
    std::vector<std::unique_ptr<IrradianceConvoluterKernel>> kernels;
    explicit IrradianceConvoluterFrame(engine::Engine* e) noexcept;
    ~IrradianceConvoluterFrame() noexcept;
};

class IrradianceConvoluter final : public Node {
private:
    std::vector<std::unique_ptr<IrradianceConvoluterFrame>> frames;
    IrradianceConvoluterFrame* frame = nullptr;
    std::shared_ptr<mesh::Mesh> meshes[6];

    void record(const mesh::Mesh* msh, IrradianceConvoluterKernel* kernel) noexcept;

public:
    IrradianceConvoluter(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~IrradianceConvoluter() noexcept final;
    void update() noexcept final;
    void record(unsigned int kernel_index) noexcept final;
    void record_continuously(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif
