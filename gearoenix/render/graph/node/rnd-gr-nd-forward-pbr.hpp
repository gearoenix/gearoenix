#ifndef GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix.hpp"
#include "../../../math/math-vector.hpp"
#include "../../buffer/rnd-buf-framed-uniform.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../pipeline/rnd-pip-forward-pbr-resource-set.hpp"
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
    class ForwardPbrResourceSet;
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
    struct ForwardPbrUniform {
        // Bring lights in here not scene
        math::Mat4x4 cascades_view_projections_bias[GX_MAX_SHADOW_CASCADES];
        core::Real cascades_count = static_cast<core::Real>(GX_MAX_SHADOW_CASCADES);
        explicit ForwardPbrUniform(const light::CascadeInfo* cas, const engine::Engine* e) noexcept;
    };

    struct ForwardPbrRenderData {
        std::unique_ptr<pipeline::ForwardPbrResourceSet> r;
        std::unique_ptr<buffer::Uniform> u;
        ForwardPbrRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
    };

    struct ForwardPbrKernel {
        std::unique_ptr<command::Buffer> secondary_cmd;
        core::OneLoopPool<ForwardPbrRenderData> render_data_pool;
        ForwardPbrKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    };

    struct ForwardPbrFrame {
        std::vector<std::unique_ptr<ForwardPbrKernel>> kernels;
        explicit ForwardPbrFrame(engine::Engine* e) noexcept;
    };

    /// This renders only one directional light with one shadow map.
    /// In here I do not care for race issues (for performance reason).
    /// The user of this class must use its functionalities in their correct contextes.
    class ForwardPbr : public Node {
    private:
        std::vector<std::unique_ptr<ForwardPbrFrame>> frames;
        ForwardPbrFrame* frame = nullptr;
		const scene::Scene* scn = nullptr;
		const camera::Camera* cam = nullptr;
		const std::vector<model::Model*>* seen_models = nullptr;
		const std::vector<std::pair<light::Directional*, light::CascadeInfo*>>* directional_lights = nullptr;

        void record(
            const scene::Scene* s,
            const camera::Camera* c,
            const light::Directional* l,
            const model::Model* m,
            const light::CascadeInfo* cas,
            unsigned int kernel_index) noexcept;

    public:
        const static unsigned int DIFFUSE_ENVIRONMENT_INDEX;
        const static unsigned int SPECULAR_ENVIRONMENT_INDEX;
        const static unsigned int AMBIENT_OCCLUSION_INDEX;
        const static unsigned int SHADOW_MAP_INDEX;
        const static unsigned int BRDFLUT_INDEX;

        ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        ~ForwardPbr() noexcept final = default;

        void set_diffuse_environment(texture::Cube* t) noexcept;
        void set_specular_environment(texture::Cube* t) noexcept;
        void set_ambient_occlusion(texture::Texture2D* t) noexcept;
        void set_shadow_mapper(texture::Texture2D* t) noexcept;
        void set_brdflut(texture::Texture2D* t) noexcept;

        /// This will be called at the start of each frame
        void update() noexcept final;
        /// Multithreaded rendering happens in here

		void set_scene(const scene::Scene* scn) noexcept;
		void set_camera(const camera::Camera* cam) noexcept;
		void set_seen_models(const std::vector<model::Model*>* models) noexcept;
		void set_directional_lights(const std::vector<std::pair<light::Directional*, light::CascadeInfo*>>* m) noexcept;
        void record(unsigned int kernel_index) noexcept;
        /// This will be called at the end of each frame for pushing jobs to GPU
        void submit() noexcept final;
    };
}
}
#endif
