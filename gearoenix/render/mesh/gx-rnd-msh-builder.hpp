#ifndef GEAROENIX_RENDER_MESH_BUILDER_HPP
#define GEAROENIX_RENDER_MESH_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../gx-rnd-vertex.hpp"
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
protected:
    const std::shared_ptr<core::ecs::EntitySharedBuilder> entity_builder;
    const std::string name;
    const math::Aabb3 occlusion_box;

    Builder(
        engine::Engine& e,
        const std::string& name,
        const std::vector<PbrVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        math::Aabb3&& occlusion_box) noexcept;

    virtual void set_material_type_index(const std::type_index& material_type) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder() noexcept;

    /// Sets the material of the mesh. It can be called only once
    template <typename T>
    void set_material(T&& mat) noexcept
    {
        entity_builder->get_builder().add_component(std::forward<T>(mat));
        set_material_type_index(std::type_index(typeid(T)));
    }
};
}
#endif