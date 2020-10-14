#ifndef GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_FORWARD_PBR_HPP
#include "../../../core/gx-cr-pool.hpp"
#include "../../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../../math/gx-math-matrix-4d.hpp"
#include "../../material/gx-rnd-mat-translucency-mode.hpp"
#include "gx-rnd-gr-nd-node.hpp"
#include <limits>
#include <vector>

namespace gearoenix::render::buffer {
class Manager;
class Uniform;
}

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::command {
class Buffer;
}

namespace gearoenix::render::light {
class Directional;
class CascadeInfo;
}

namespace gearoenix::render::model {
class Model;
class Mesh;
}

namespace gearoenix::render::pipeline {
class ForwardPbrResourceSet;
}

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::render::sync {
class Semaphore;
}

namespace gearoenix::render::texture {
class Texture;
class Texture2D;
class TextureCube;
}

namespace gearoenix::render::graph::node {
struct ForwardPbrUniform {
    math::Vec4<float> point_lights_color_min_radius[GX_MAX_POINT_LIGHTS];
    math::Vec4<float> point_lights_position_max_radius[GX_MAX_POINT_LIGHTS];
    math::Mat4x4<float> shadow_caster_directional_lights_cascades_view_projections_bias[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER * GX_MAX_SHADOW_CASCADES];
    math::Vec4<float> shadow_caster_directional_lights_color_cascades_count[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER];
    math::Vec4<float> shadow_caster_directional_lights_direction[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER];
    float point_lights_count = 0.0f;
    float shadow_caster_directional_lights_count = 0.0f;
};

struct ForwardPbrRenderData {
    std::unique_ptr<pipeline::ForwardPbrResourceSet> r;
    std::shared_ptr<buffer::Uniform> u;
    ForwardPbrRenderData(engine::Engine* e, pipeline::Pipeline* pip, std::size_t frame_number) noexcept;
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
/// The user of this class must use its functionality in their correct contexts.
class ForwardPbr final : public Node {
private:
    std::vector<std::unique_ptr<ForwardPbrFrame>> frames;
    ForwardPbrFrame* frame = nullptr;
    std::size_t frame_number = std::numeric_limits<std::size_t>::max();
    const scene::Scene* scn = nullptr;
    const camera::Camera* cam = nullptr;
    std::vector<std::pair<const model::Model*, const model::Mesh*>> meshes;
    std::vector<const light::CascadeInfo*> cascades;
    unsigned int directional_light_shadow_caster_index = 0;
    unsigned int shadow_map_index = 0;
    ForwardPbrUniform uniform;

    void record(
        const model::Model* mdl,
        const model::Mesh* msh,
        const ForwardPbrUniform& u,
        ForwardPbrKernel* kernel) noexcept;

public:
    const static unsigned int DIFFUSE_ENVIRONMENT_INDEX;
    const static unsigned int SPECULAR_ENVIRONMENT_INDEX;
    const static unsigned int AMBIENT_OCCLUSION_INDEX;
    const static unsigned int BRDFLUT_INDEX;
    const static unsigned int SHADOW_MAP_000_INDEX;

    ForwardPbr(
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~ForwardPbr() noexcept final;

    void set_diffuse_environment(texture::TextureCube* t) noexcept;
    void set_specular_environment(texture::TextureCube* t) noexcept;
    void set_ambient_occlusion(texture::Texture2D* t) noexcept;
    void set_brdflut(texture::Texture2D* t) noexcept;

    /// This will be called at the start of each frame
    void update() noexcept final;
    void set_scene(const scene::Scene* scn) noexcept;
    void set_camera(const camera::Camera* cam) noexcept;
    void add_mesh(std::pair<const model::Model*, const model::Mesh*> msh) noexcept;
    void add_cascade(const light::CascadeInfo* c) noexcept;
    /// Multithreaded rendering happens in here
    void record(unsigned int kernel_index) noexcept final;
    void record_continuously(unsigned int kernel_index) noexcept final;
    /// This will be called at the end of each frame for pushing jobs to GPU
    void submit() noexcept final;
};
}
#endif
