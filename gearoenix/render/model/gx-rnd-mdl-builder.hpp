#ifndef GEAROENIX_RENDER_MODEL_BUILDER_HPP
#define GEAROENIX_RENDER_MODEL_BUILDER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../material/gx-rnd-mat-pbr.hpp"
#include <memory>
#include <string>
#include <typeindex>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Builder {
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);
    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, bound_mesh);
    GX_GET_CVAL_PRT(bool, is_transformable);

private:
    void set_material_type_index(const std::type_index& in_material_type) noexcept;

protected:
    Builder(
        engine::Engine& e,
        const std::string& name,
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        bool is_transformable = true) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder() noexcept;

    /// Sets the material of the mesh. It can be called only once and only one type of material must be set
    virtual void set_material(const material::Pbr& mat) noexcept;
    [[nodiscard]] physics::Transformation& get_transformation() noexcept;
};
}
#endif