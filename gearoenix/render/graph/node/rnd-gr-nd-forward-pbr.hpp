#ifndef GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix.hpp"
#include "../../../math/math-vector.hpp"
#include "../../material/rnd-mat-translucency-mode.hpp"
#include "rnd-gr-nd-node.hpp"
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
namespace mesh {
    class Mesh;
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
        math::Vec4 point_lights_color_min_radius[GX_MAX_POINT_LIGHTS] = {};
        math::Vec4 point_lights_position_max_radius[GX_MAX_POINT_LIGHTS] = {};
        math::Mat4x4 shadow_caster_directional_lights_cascades_view_projections_bias[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER * GX_MAX_SHADOW_CASCADES] = {};
        math::Vec4 shadow_caster_directional_lights_color_cascades_count[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER] = {};
        math::Vec4 shadow_caster_directional_lights_direction[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER] = {};
        core::Real point_lights_count = 0.0f;
        core::Real shadow_caster_directional_lights_count = 0.0f;
        explicit ForwardPbrUniform(
            const std::map<core::Real, std::map<light::Directional*, light::CascadeInfo*>>* directional_lights,
            const scene::Scene* scn, const model::Model* mdl) noexcept;
    };

    struct ForwardPbrRenderData {
        std::unique_ptr<pipeline::ForwardPbrResourceSet> r;
        std::unique_ptr<buffer::Uniform> u;
        ForwardPbrRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept;
        ~ForwardPbrRenderData() noexcept;
    };

    struct ForwardPbrKernel {
        std::unique_ptr<command::Buffer> secondary_cmd;
        core::OneLoopPool<ForwardPbrRenderData> render_data_pool;
        ForwardPbrKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
        ~ForwardPbrKernel() noexcept;
    };

    struct ForwardPbrFrame {
        std::vector<std::unique_ptr<ForwardPbrKernel>> kernels;
        explicit ForwardPbrFrame(engine::Engine* e) noexcept;
        ~ForwardPbrFrame() noexcept;
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
        const std::vector<std::map<model::Model*, mesh::Mesh*>*> models = nullptr;
        const std::map<core::Real, std::map<light::Directional*, light::CascadeInfo*>>* directional_lights = nullptr;

        void record(
            const model::Model* m,
            unsigned int kernel_index,
            material::TranslucencyMode translucency_mode = material::TranslucencyMode::Opaque) noexcept;

    public:
        const static unsigned int DIFFUSE_ENVIRONMENT_INDEX;
        const static unsigned int SPECULAR_ENVIRONMENT_INDEX;
        const static unsigned int AMBIENT_OCCLUSION_INDEX;
        const static unsigned int BRDFLUT_INDEX;
        const static unsigned int SHADOW_MAP_000_INDEX;

        ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        ~ForwardPbr() noexcept final;

        void set_diffuse_environment(texture::Cube* t) noexcept;
        void set_specular_environment(texture::Cube* t) noexcept;
        void set_ambient_occlusion(texture::Texture2D* t) noexcept;
        void set_brdflut(texture::Texture2D* t) noexcept;

        /// This will be called at the start of each frame
        void update() noexcept final;
        /// Multithreaded rendering happens in here

        void set_scene(const scene::Scene* scn) noexcept;
        void set_camera(const camera::Camera* cam) noexcept;
        void add_models(const std::map<model::Model*, mesh::Mesh*>* models) noexcept;
        void set_directional_lights(const std::map<core::Real, std::map<light::Directional*, light::CascadeInfo*>>* m) noexcept;
        void record(unsigned int kernel_index) noexcept;
        /// This will be called at the end of each frame for pushing jobs to GPU
        void submit() noexcept final;
    };
}
}
#endif
