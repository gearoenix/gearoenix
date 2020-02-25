#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/cr-types.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-gr-tr-tree.hpp"
#include "../../material/rnd-mat-type.hpp"
#include <map>
#include <set>
#include <tuple>
#include <vector>

namespace gearoenix::render::graph::node {
class ForwardPbr;
class Node;
class Unlit;
class SkyboxEquirectangular;
class SkyboxCube;
}

namespace gearoenix::render::light {
class CascadeInfo;
}

namespace gearoenix::render::model {
    class Model;
    class Mesh;
}

namespace gearoenix::render::scene {
    class Scene;
}

namespace gearoenix::render::camera {
class Camera;
}
namespace gearoenix::render::graph::tree {
class Pbr final : public Tree {
public:
    struct Nodes {
        node::ForwardPbr* forward_pbr = nullptr;
        node::Unlit* unlit = nullptr;
    };
    struct CameraData {
        std::map<core::Real, std::vector<node::Node*>> skyboxes;
        Nodes opaques {};
        std::vector<node::Node*> transparencies;

        void clear() noexcept ;
    };
    using SceneData = std::map<core::Real, std::map<const camera::Camera*, CameraData>>;

private:
    bool in_weak_hardware = true;
    core::OneLoopPool<node::ForwardPbr> forward_pbr;
    //    core::OneLoopPool<node::SkyCube> skybox_cube;
    core::OneLoopPool<node::SkyboxEquirectangular> skybox_equirectangular;
    core::OneLoopPool<node::Unlit> unlit;
    std::map<core::Real, std::map<const scene::Scene*, SceneData>> nodes;
    std::vector<light::CascadeInfo*> cascades;

    void update_skyboxes(const scene::Scene* scn, const camera::Camera* cam, CameraData& camera_nodes) noexcept;
    void update_opaque(
            const std::vector<std::tuple<material::Type, model::Model*, model::Mesh*>>& seen_meshes,
            const scene::Scene* scn, const camera::Camera* cam, CameraData& camera_nodes) noexcept;
    void update_transparent(
            const std::vector<std::tuple<core::Real, material::Type, model::Model*, model::Mesh*>>& seen_meshes,
            const scene::Scene* scn, const camera::Camera* cam, CameraData& camera_nodes) noexcept;

public:
    Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~Pbr() noexcept final;
    void update() noexcept final;
    void record(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif