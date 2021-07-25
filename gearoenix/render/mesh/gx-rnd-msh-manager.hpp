#ifndef GEAROENIX_RENDER_MESH_MANAGER_HPP
#define GEAROENIX_RENDER_MESH_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-vertex.hpp"
#include "../texture/gx-rnd-txt-face.hpp"
#include <memory>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
struct Builder;
struct Manager {
private:
    engine::Engine& e;

    std::weak_ptr<Mesh> icosphere;
    std::weak_ptr<Mesh> plate;
    std::weak_ptr<Mesh> cube;
    std::weak_ptr<Mesh> inward_cube;
    std::map<texture::Face, std::weak_ptr<Mesh>> face_squares;

public:
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept = default;

    [[nodiscard]] std::shared_ptr<Builder> build_icosphere(const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Builder> build_plate(const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Builder> build_cube(const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Builder> build_inward_cube(const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] std::shared_ptr<Builder> build_face_square(texture::Face f, const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;

    [[nodiscard]] virtual std::shared_ptr<Builder> build(
        const std::string& name,
        const std::vector<PbrVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        const math::Aabb3& occlusion_box,
        const core::sync::EndCallerIgnored& c) noexcept = 0;

    [[nodiscard]] std::shared_ptr<Builder> build(
        const std::string& name,
        const std::vector<PbrVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
};
}

#endif
