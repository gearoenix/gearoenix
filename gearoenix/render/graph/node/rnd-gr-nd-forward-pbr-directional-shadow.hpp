#ifndef GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix.hpp"
#include "../../../math/math-vector.hpp"
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
    class ForwardPbrDirectionalShadow;
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
    struct ForwardPbrDirectionalShadowKernel {
        std::shared_ptr<command::Buffer> secondary_cmd = nullptr;
        core::OneLoopPool<pipeline::ForwardPbrDirectionalShadowResourceSet> render_data_pool;
        ForwardPbrDirectionalShadowKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    };

    struct ForwardPbrDirectionalShadowFrame {
        std::shared_ptr<command::Buffer> primary_cmd = nullptr;
        std::shared_ptr<sync::Semaphore> semaphore = nullptr;
        std::vector<std::shared_ptr<ForwardPbrDirectionalShadowKernel>> kernels;
        explicit ForwardPbrDirectionalShadowFrame(engine::Engine* e) noexcept;
    };

    /// This renders only one directional light with one shadow map.
    /// In here I do not care for race issues (for performance reason).
    /// The user of this class must use its functionalities in their correct contextes.
    class ForwardPbrDirectionalShadow : public Node {
    private:
        std::vector<std::shared_ptr<ForwardPbrDirectionalShadowFrame>> frames;
        ForwardPbrDirectionalShadowFrame* frame = nullptr;

    public:
        const static unsigned int diffuse_environment_index;
        const static unsigned int specular_environment_index;
        const static unsigned int ambient_occlusion_index;
        const static unsigned int shadow_map_index;
        const static unsigned int brdflut_index;

        ForwardPbrDirectionalShadow(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        ~ForwardPbrDirectionalShadow() noexcept final = default;

        void set_diffuse_environment(const std::shared_ptr<texture::Cube>& t) noexcept;
        void set_specular_environment(const std::shared_ptr<texture::Cube>& t) noexcept;
        void set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t) noexcept;
        void set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t) noexcept;
        void set_brdflut(const std::shared_ptr<texture::Texture2D>& t) noexcept;

        /// This will be called at the start of each frame
        void update() noexcept;
        /// Multithreaded rendering happens in here
        void record(
            const scene::Scene* s,
            const camera::Camera* c,
            const light::Directional* l,
            const model::Model* m,
            unsigned int kernel_index) noexcept;
        /// This will be called at the end of each frame for pushing jobs to GPU
        void submit() noexcept;
    };
}
}
#endif
