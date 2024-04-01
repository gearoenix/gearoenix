#ifndef GEAROENIX_RENDER_MESH_MANAGER_HPP
#define GEAROENIX_RENDER_MESH_MANAGER_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-vertex.hpp"
#include "../texture/gx-rnd-txt-face.hpp"
#include <map>
#include <memory>
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
struct Manager {
protected:
    engine::Engine& e;
    std::mutex meshes_lock;
    std::map<std::string, std::weak_ptr<Mesh>> meshes;

    explicit Manager(engine::Engine& e);

public:
    virtual ~Manager();

    void build_icosphere(std::size_t subdivisions, core::job::EndCallerShared<Mesh>&& end_callback);
    void build_plate(core::job::EndCallerShared<Mesh>&& end_callback);
    void build_cube(core::job::EndCallerShared<Mesh>&& end_callback);
    void build_inward_cube(core::job::EndCallerShared<Mesh>&& end_callback);
    // void build_face_square(texture::Face f, core::job::EndCallerShared<Mesh>&& end_callback);

    virtual void build(
        std::string&& name,
        Vertices&& vertices,
        std::vector<std::uint32_t>&& indices,
        const math::Aabb3<double>& occlusion_box,
        core::job::EndCallerShared<Mesh>&& end_callback)
        = 0;

    void build(
        std::string&& name,
        std::vector<PbrVertex>&& vertices,
        std::vector<std::uint32_t>&& indices,
        core::job::EndCallerShared<Mesh>&& end_callback);

    void build(
        std::string&& name,
        std::vector<PbrVertexAnimated>&& vertices,
        std::vector<std::uint32_t>&& indices,
        core::job::EndCallerShared<Mesh>&& end_callback);

    [[nodiscard]] bool remove_if_exist(const std::string& name);
};
}

#endif
