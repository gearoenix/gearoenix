#ifndef GEAROENIX_RENDER_GRAPH_NODE_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_SKYBOX_EQUIRECTANGULAR_HPP
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

namespace gearoenix::render::skybox {
class Equirectangular;
}

namespace gearoenix::render::pipeline {
class SkyboxEquirectangularResourceSet;
}

namespace gearoenix::render::graph::node {

struct SkyboxEquirectangularUniform {
    math::Mat4x4<float> mvp;
};

struct SkyboxEquirectangularRenderData {
    std::unique_ptr<pipeline::SkyboxEquirectangularResourceSet> r;
    std::unique_ptr<buffer::Uniform> u;
    SkyboxEquirectangularRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
    ~SkyboxEquirectangularRenderData() noexcept;
};

struct SkyboxEquirectangularKernel {
    std::unique_ptr<command::Buffer> secondary_cmd;
    core::OneLoopPool<SkyboxEquirectangularRenderData> render_data_pool;
    SkyboxEquirectangularKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~SkyboxEquirectangularKernel() noexcept;
};

struct SkyboxEquirectangularFrame {
    std::vector<std::unique_ptr<SkyboxEquirectangularKernel>> kernels;
    explicit SkyboxEquirectangularFrame(engine::Engine* e) noexcept;
    ~SkyboxEquirectangularFrame() noexcept;
};

class SkyboxEquirectangular final : public Node {
private:
    std::vector<std::unique_ptr<SkyboxEquirectangularFrame>> frames;
    SkyboxEquirectangularFrame* frame = nullptr;
    const camera::Camera* cam = nullptr;
    std::vector<const skybox::Equirectangular*> skies;

    void record_sky(const skybox::Equirectangular* sky, const SkyboxEquirectangularUniform& u, SkyboxEquirectangularKernel* kernel) noexcept;

public:
    SkyboxEquirectangular(
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~SkyboxEquirectangular() noexcept final;
    void update() noexcept final;
    void set_camera(const camera::Camera* cam) noexcept;
    void add_sky(const skybox::Equirectangular* sky) noexcept;
    void record_continuously(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif
