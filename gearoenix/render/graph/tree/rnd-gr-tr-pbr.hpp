#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/cr-types.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../material/rnd-mat-type.hpp"
#include "rnd-gr-tr-tree.hpp"
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
class IrradianceConvoluter;
class RadianceConvoluter;
class MipmapGenerator;
}

namespace gearoenix::render::light {
class CascadeInfo;
}

namespace gearoenix::render::model {
class Model;
class Mesh;
}

namespace gearoenix::render::reflection {
class Runtime;
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
        std::map<double, std::vector<node::Node*>> skyboxes;
        Nodes opaques {};
        std::vector<node::Node*> transparencies;

        void clear() noexcept;
    };

    struct RuntimeReflectionFaceData {
        // environment related stuff
        camera::Camera* cam = nullptr;
        CameraData camera_data;
        node::IrradianceConvoluter* irradiance = nullptr;
        std::vector<node::RadianceConvoluter*> radiances;

        explicit RuntimeReflectionFaceData(std::size_t radiance_levels) noexcept
            : radiances(radiance_levels)
        {
        }
    };

    struct RuntimeReflectionData {
        node::MipmapGenerator* environment_mipmap_generator = nullptr;
        node::MipmapGenerator* irradiance_mipmap_generator = nullptr;
        RuntimeReflectionFaceData faces[6];

        explicit RuntimeReflectionData(std::size_t radiance_levels) noexcept
            : faces {
                RuntimeReflectionFaceData(radiance_levels),
                RuntimeReflectionFaceData(radiance_levels),
                RuntimeReflectionFaceData(radiance_levels),
                RuntimeReflectionFaceData(radiance_levels),
                RuntimeReflectionFaceData(radiance_levels),
                RuntimeReflectionFaceData(radiance_levels),
            }
        {
        }
    };

    struct SceneData {
        std::map<double, std::map<const camera::Camera*, CameraData>> cameras;
    };

private:
    bool in_weak_hardware = true;
    core::OneLoopPool<node::ForwardPbr> forward_pbr;
    core::OneLoopPool<node::SkyboxCube> skybox_cube;
    core::OneLoopPool<node::SkyboxEquirectangular> skybox_equirectangular;
    core::OneLoopPool<node::Unlit> unlit;
    std::map<double, std::map<const scene::Scene*, SceneData>> nodes;
    std::map<reflection::Runtime*, RuntimeReflectionData> runtime_reflections_data;
    std::vector<light::CascadeInfo*> cascades;

    void update_camera(const scene::Scene* scn, camera::Camera* cam, CameraData& camera_nodes) noexcept;
    void update_skyboxes(const scene::Scene* scn, const camera::Camera* cam, CameraData& camera_nodes) noexcept;
    void update_runtime_reflection(const scene::Scene* scn) noexcept;
    void update_opaque(
        const std::vector<std::tuple<double, material::Type, model::Model*, model::Mesh*>>& seen_meshes,
        const scene::Scene* scn, const camera::Camera* cam, CameraData& camera_nodes) noexcept;
    void update_transparent(
        const std::vector<std::tuple<double, material::Type, model::Model*, model::Mesh*>>& seen_meshes,
        const scene::Scene* scn, const camera::Camera* cam, CameraData& camera_nodes) noexcept;

    void record_runtime_reflection(
        const scene::Scene* scn, unsigned int& task_number, unsigned int kernel_index,
        unsigned int kernels_count) noexcept;

    void submit_camera_data(const CameraData& camera_data) const noexcept;
    void submit_runtime_reflections(const scene::Scene* scn) noexcept;

public:
    Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~Pbr() noexcept final;
    void update() noexcept final;
    void record(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif