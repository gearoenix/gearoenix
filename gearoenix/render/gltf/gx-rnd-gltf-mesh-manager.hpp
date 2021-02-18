#ifndef GEAROENIX_RENDER_GLTF_MESH_MANAGER_HPP
#define GEAROENIX_RENDER_GLTF_MESH_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <map>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::gltf {
struct Loader;
struct MeshManager final {
private:
    Loader& loader;
    std::map<std::size_t, std::weak_ptr<mesh::Mesh>> loaded_meshes;
    std::map<std::string, std::size_t> name_to_index;

public:
    explicit MeshManager(Loader& loader) noexcept;
    ~MeshManager() noexcept;

    void get(std::size_t, std::shared_ptr<core::ecs::EntitySharedBuilder>, const core::sync::EndCallerIgnored&) noexcept;
};
}

#endif