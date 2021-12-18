#ifndef GEAROENIX_RENDER_MESH_BUILDER_HPP
#define GEAROENIX_RENDER_MESH_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-vertex.hpp"
#include "../material/gx-rnd-mat-pbr.hpp"
#include <memory>
#include <optional>
#include <string>
#include <typeindex>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
/// TODO add instancing later
struct Builder {
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder)
    GX_GET_REFC_PRT(std::string, name)
    GX_GET_REFC_PRT(math::Aabb3, occlusion_box)

private:
    void set_material_type_index(const std::type_index& in_material_type) noexcept;

protected:
    Builder(
        engine::Engine& e,
        const std::string& name,
        math::Aabb3&& occlusion_box) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder() noexcept;

    /// Sets the material of the mesh. It can be called only once, nd only one type of material must be set
    virtual void set_material(const material::Pbr& mat) noexcept;
};
}
#endif