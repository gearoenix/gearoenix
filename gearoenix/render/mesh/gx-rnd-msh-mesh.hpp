#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include <memory>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::mesh {
struct Mesh {
protected:
    Mesh() noexcept = default;

public:
    Mesh(Mesh&&) = delete;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    virtual ~Mesh() noexcept = default;
    virtual void set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept = 0;
};
}
#endif
