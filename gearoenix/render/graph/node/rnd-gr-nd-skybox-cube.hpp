#ifndef GEAROENIX_RENDER_GRAPH_NODE_SKYBOX_CUBE_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_SKYBOX_CUBE_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix-4d.hpp"
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

namespace gearoenix::render::skybox {
class Cube;
}

namespace gearoenix::render::pipeline {
class SkyboxCubeResourceSet;
}

namespace gearoenix::render::graph::node {

struct SkyboxCubeUniform {
    math::Mat4x4<float> mvp;
};

struct SkyboxCubeRenderData {
    std::unique_ptr<pipeline::SkyboxCubeResourceSet> r;
    std::unique_ptr<buffer::Uniform> u;
    SkyboxCubeRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
    ~SkyboxCubeRenderData() noexcept;
};

struct SkyboxCubeKernel {
    std::unique_ptr<command::Buffer> secondary_cmd;
    core::OneLoopPool<SkyboxCubeRenderData> render_data_pool;
    SkyboxCubeKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~SkyboxCubeKernel() noexcept;
};

struct SkyboxCubeFrame {
    std::vector<std::unique_ptr<SkyboxCubeKernel>> kernels;
    explicit SkyboxCubeFrame(engine::Engine* e) noexcept;
    ~SkyboxCubeFrame() noexcept;
};

class SkyboxCube final : public Node {
private:
    std::vector<std::unique_ptr<SkyboxCubeFrame>> frames;
    SkyboxCubeFrame* frame = nullptr;
    const camera::Camera* cam = nullptr;
    std::vector<const skybox::Cube*> skies;

    void record_sky(const skybox::Cube* sky, const SkyboxCubeUniform& u, SkyboxCubeKernel* kernel) noexcept;

public:
    SkyboxCube(
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~SkyboxCube() noexcept final;
    void update() noexcept final;
    void set_camera(const camera::Camera* cam) noexcept;
    void add_sky(const skybox::Cube* sky) noexcept;
    void record_continuously(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif
