#ifndef GEAROENIX_RENDER_MESH_MANAGER_HPP
#define GEAROENIX_RENDER_MESH_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-vertex.hpp"
#include "../texture/rnd-txt-face.hpp"
#include <memory>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::mesh {
class Mesh;
class Manager {
private:
    engine::Engine* const e;
    core::cache::File<Mesh> cache;
    std::weak_ptr<Mesh> icosphere;
    std::weak_ptr<Mesh> plate;
    std::weak_ptr<Mesh> cube;
    std::weak_ptr<Mesh> inward_cube;
    std::map<texture::Face, std::weak_ptr<Mesh>> face_squares;

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept = default;
    [[nodiscard]] std::shared_ptr<Mesh> get_gx3d(core::Id id, core::sync::EndCaller<Mesh>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> create_icosphere(core::sync::EndCaller<Mesh>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> create_plate(core::sync::EndCaller<Mesh>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> create_cube(core::sync::EndCaller<Mesh>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> create_inward_cube(core::sync::EndCaller<Mesh>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> create_face_square(texture::Face f, core::sync::EndCaller<Mesh>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> create(
        std::string name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        const math::Aabb3& occlusion_box,
        core::sync::EndCaller<Mesh>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Mesh> create(
        std::string name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<Mesh>& c) noexcept;
};
}

#endif
