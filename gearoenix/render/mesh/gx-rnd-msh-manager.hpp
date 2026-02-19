#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-vertex.hpp"
#include "../texture/gx-rnd-txt-face.hpp"
#include <map>
#include <memory>
#include <vector>

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Buffer;
struct Mesh;
struct Manager : core::Singleton<Manager> {
protected:
    // TODO: it must use the new structure for caching logic
    std::mutex buffers_lock;
    std::map<std::string, std::weak_ptr<Buffer>> buffers;

    Manager();

    virtual void build(std::string&& name, Vertices&& vertices, std::vector<std::uint32_t>&& indices, const math::Aabb3<double>& occlusion_box, core::job::EndCallerShared<Buffer>&& end_callback) = 0;

    virtual void build(std::string&& name, std::shared_ptr<Buffer>&& buffer, std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback) = 0;

public:
    ~Manager() override;

    void build_icosphere(std::uint64_t subdivisions, std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback);
    void build_plate(std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback);
    void build_cube(std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback);
    void build_inward_cube(std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback);
    // void build_face_square(texture::Face f, core::job::EndCallerShared<Mesh>&& end_callback);

    void build(std::string&& name, Vertices&& vertices, std::vector<std::uint32_t>&& indices, const math::Aabb3<double>& occlusion_box, std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback);

    void build(std::string&& name, std::vector<PbrVertex>&& vertices, std::vector<std::uint32_t>&& indices, std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback);

    void build(std::string&& name, std::vector<PbrVertexAnimated>&& vertices, std::vector<std::uint32_t>&& indices, std::shared_ptr<material::Material>&& material, core::job::EndCallerShared<Mesh>&& end_callback);

    [[nodiscard]] bool remove_if_exist(const std::string& name);
};
}