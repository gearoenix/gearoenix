#ifndef GEAROENIX_RENDER_GRAPH_NODE_SHADOW_MAPPER_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_SHADOW_MAPPER_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix.hpp"
#include "rnd-gr-nd-node.hpp"
#include <memory>
#include <vector>

namespace gearoenix::render {
namespace buffer {
    class Uniform;
    class Manager;
}
namespace camera {
    class Camera;
}
namespace command {
    class Buffer;
}
namespace light {
    class Directional;
}
namespace model {
    class Model;
    class Mesh;
}
namespace pipeline {
    class ShadowMapperResourceSet;
}
namespace scene {
    class Scene;
}
namespace texture {
    class Texture;
    class Texture2D;
    class TextureCube;
}
namespace graph::node {
    struct ShadowMapperUniform {
        math::Mat4x4 mvp;
        core::Real alpha;
        core::Real alpha_cutoff;
    };

    struct ShadowMapperRenderData {
        std::unique_ptr<pipeline::ShadowMapperResourceSet> r;
        std::unique_ptr<buffer::Uniform> u;
        ShadowMapperRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
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
    /// The user of this class must use its functionality in their correct contexts.
    class ShadowMapper : public Node {
    private:
        std::vector<std::unique_ptr<ShadowMapperFrame>> frames;
        ShadowMapperFrame* frame = nullptr;

    public:
        ShadowMapper(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        ~ShadowMapper() noexcept final;
        /// This will be called at the start of each frame
        void update() noexcept final;
        /// Multithreaded rendering happens in here
        void record_shadow(const math::Mat4x4& mvp, const model::Model* m, std::size_t kernel_index) noexcept;
        /// This will be called at the end of each frame for pushing jobs to GPU
        void submit() noexcept final;
    };
}
}
#endif
