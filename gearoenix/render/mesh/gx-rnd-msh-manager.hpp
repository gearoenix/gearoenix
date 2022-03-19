#ifndef GEAROENIX_RENDER_MESH_MANAGER_HPP
#define GEAROENIX_RENDER_MESH_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
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
    GX_CREATE_GUARD(meshes);
    std::map<std::string, std::weak_ptr<Mesh>> meshes;

    explicit Manager(engine::Engine& e) noexcept;

public:
    virtual ~Manager() noexcept;

    [[nodiscard]] std::shared_ptr<Mesh> build_sphere(core::sync::EndCallerIgnored&& end_callback = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> build_plate(core::sync::EndCallerIgnored&& end_callback = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> build_cube(core::sync::EndCallerIgnored&& end_callback = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> build_inward_cube(core::sync::EndCallerIgnored&& end_callback = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> build_face_square(texture::Face f, core::sync::EndCallerIgnored&& end_callback = GX_DEFAULT_IGNORED_END_CALLER) noexcept;

    [[nodiscard]] virtual std::shared_ptr<Mesh> build(
        std::string&& name,
        std::vector<PbrVertex>&& vertices,
        std::vector<std::uint32_t>&& indices,
        math::Aabb3<double>&& occlusion_box,
        core::sync::EndCallerIgnored&& end_callback) noexcept = 0;

    [[nodiscard]] std::shared_ptr<Mesh> build(
        std::string&& name,
        std::vector<PbrVertex>&& vertices,
        std::vector<std::uint32_t>&& indices,
        core::sync::EndCallerIgnored&& end_callback = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
};
}

#endif
