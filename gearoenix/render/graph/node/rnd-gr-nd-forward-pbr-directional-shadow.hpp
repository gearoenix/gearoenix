#ifndef GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix.hpp"
#include "../../../math/math-vector.hpp"
#include "../../buffer/rnd-buf-framed-uniform.hpp"
#include "rnd-gr-nd-node.hpp"
#include <memory>
#include <vector>

namespace gearoenix::render {
namespace buffer {
    class Manager;
    class Uniform;
}
namespace camera {
    class Camera;
}
namespace command {
    class Buffer;
}
namespace light {
    class Directional;
    class CascadeInfo;
}
namespace model {
    class Model;
    class Mesh;
}
namespace pipeline {
    class ForwardPbrDirectionalShadowResourceSet;
}
namespace scene {
    class Scene;
}
namespace sync {
    class Semaphore;
}
namespace texture {
    class Cube;
    class Texture;
    class Texture2D;
}
namespace graph::node {
    struct ForwardPbrDirectionalShadowUniform {
        // Bring lights in here not scene
        math::Mat4x4 cascades_view_projections_bias[GX_MAX_SHADOW_CASCADES];
        core::Real cascades_count = static_cast<core::Real>(GX_MAX_SHADOW_CASCADES);
        explicit ForwardPbrDirectionalShadowUniform(const light::CascadeInfo* cas) noexcept;
    };

    struct ForwardPbrDirectionalShadowRenderData {
        std::unique_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet> r;
        std::unique_ptr<buffer::Uniform> u;
        ForwardPbrDirectionalShadowRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
    };

    struct ForwardPbrDirectionalShadowKernel {
        std::unique_ptr<command::Buffer> secondary_cmd;
        core::OneLoopPool<ForwardPbrDirectionalShadowRenderData> render_data_pool;
        ForwardPbrDirectionalShadowKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    };

    struct ForwardPbrDirectionalShadowFrame {
        std::vector<std::unique_ptr<ForwardPbrDirectionalShadowKernel>> kernels;
        explicit ForwardPbrDirectionalShadowFrame(engine::Engine* e) noexcept;
    };

    /// This renders only one directional light with one shadow map.
    /// In here I do not care for race issues (for performance reason).
    /// The user of this class must use its functionalities in their correct contextes.
    class ForwardPbrDirectionalShadow : public Node {
    private:
        std::vector<std::unique_ptr<ForwardPbrDirectionalShadowFrame>> frames;
        ForwardPbrDirectionalShadowFrame* frame = nullptr;

    public:
        const static unsigned int DIFFUSE_ENVIRONMENT_INDEX;
        const static unsigned int SPECULAR_ENVIRONMENT_INDEX;
        const static unsigned int AMBIENT_OCCLUSION_INDEX;
        const static unsigned int SHADOW_MAP_INDEX;
        const static unsigned int BRDFLUT_INDEX;

        ForwardPbrDirectionalShadow(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        ~ForwardPbrDirectionalShadow() noexcept final = default;

        void set_diffuse_environment(const std::shared_ptr<texture::Cube>& t) noexcept;
        void set_specular_environment(const std::shared_ptr<texture::Cube>& t) noexcept;
        void set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t) noexcept;
        void set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t) noexcept;
        void set_brdflut(const std::shared_ptr<texture::Texture2D>& t) noexcept;

        /// This will be called at the start of each frame
        void update() noexcept final;
        /// Multithreaded rendering happens in here
        void record(
            const scene::Scene* s,
            const camera::Camera* c,
            const light::Directional* l,
            const model::Model* m,
            const light::CascadeInfo* cas,
            unsigned int kernel_index) noexcept;
        /// This will be called at the end of each frame for pushing jobs to GPU
        void submit() noexcept final;
    };
}
}
#endif
