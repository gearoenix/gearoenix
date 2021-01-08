#ifndef GEAROENIX_RENDER_GRAPH_NODE_SHADOW_MAPPER_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_SHADOW_MAPPER_HPP
#include "../../../core/gx-cr-pool.hpp"
#include "../../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../../math/gx-math-matrix-4d.hpp"
#include "gx-rnd-gr-nd-node.hpp"
#include <limits>
#include <memory>
#include <vector>

namespace gearoenix::render::buffer {
struct Uniform;
struct Manager;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::command {
struct Buffer;
}

namespace gearoenix::render::light {
struct Directional;
}

namespace gearoenix::render::model {
struct Model;
struct Mesh;
}

namespace gearoenix::render::pipeline {
struct ShadowMapperResourceSet;
}

namespace gearoenix::render::scene {
struct Scene;
}

namespace gearoenix::render::texture {
struct Texture;
struct Texture2D;
struct TextureCube;
struct Target;
}

namespace gearoenix::render::graph::node {
struct ShadowMapperUniform {
    math::Mat4x4<float> mvp;
    float alpha = 8888.0f; // this is for finding bugs
    float alpha_cutoff = 8888.0f;
};

struct ShadowMapperRenderData {
    std::unique_ptr<pipeline::ShadowMapperResourceSet> r;
    std::shared_ptr<buffer::Uniform> u;
    ShadowMapperRenderData(engine::Engine* e, pipeline::Pipeline* pip, std::size_t frame_number) noexcept;
    ~ShadowMapperRenderData() noexcept;
};

struct ShadowMapperKernel {
    std::unique_ptr<command::Buffer> secondary_cmd;
    core::OneLoopPool<ShadowMapperRenderData> render_data_pool;
    ShadowMapperKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~ShadowMapperKernel() noexcept;
};

struct ShadowMapperFrame {
    std::vector<std::unique_ptr<ShadowMapperKernel>> kernels;
    explicit ShadowMapperFrame(engine::Engine* e) noexcept;
    ~ShadowMapperFrame() noexcept;
};

/// This renders only one directional light with one shadow map.
/// In here I do not care for race issues (for performance reason).
/// The user of this struct must use its functionality in their correct contexts.
struct ShadowMapper final : public Node {
private:
    std::vector<std::unique_ptr<ShadowMapperFrame>> frames;
    std::shared_ptr<texture::Target> shadow_map_render_target;
    ShadowMapperFrame* frame = nullptr;
    std::size_t frame_number = std::numeric_limits<std::size_t>::max();

public:
    ShadowMapper(std::string name, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~ShadowMapper() noexcept final;
    /// This will be called at the start of each frame
    void update() noexcept final;
    /// Multithreaded rendering happens in here
    void record_shadow(const math::Mat4x4<double>& mvp, const model::Model* m, std::size_t kernel_index) noexcept;
    /// This will be called at the end of each frame for pushing jobs to GPU
    void submit() noexcept final;
};
}

#endif
