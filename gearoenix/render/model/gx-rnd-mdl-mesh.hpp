#ifndef GEAROENIX_RENDER_MODEL_MESH_HPP
#define GEAROENIX_RENDER_MODEL_MESH_HPP
#include "../../core/gx-cr-static.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Mesh {
    GX_GET_CREF_PRV(std::shared_ptr<mesh::Mesh>, msh)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, mat)
public:
    Mesh(
        platform::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

    Mesh(
        std::shared_ptr<mesh::Mesh> msh,
        std::shared_ptr<material::Material> mat) noexcept;

    ~Mesh() noexcept;

    void update() noexcept;
    [[nodiscard]] Mesh* clone() const noexcept;
};
}
#endif
