#ifndef GEAROENIX_RENDER_MODEL_MESH_HPP
#define GEAROENIX_RENDER_MODEL_MESH_HPP
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::material {
class Material;
}

namespace gearoenix::render::mesh {
class Mesh;
}

namespace gearoenix::render::model {
class Mesh {
    GX_GET_CREF_PRV(std::shared_ptr<mesh::Mesh>, msh)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, mat)
public:
    Mesh(
        system::stream::Stream* f,
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
