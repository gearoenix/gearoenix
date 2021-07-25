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

namespace gearoenix::core::ecs {
struct World;
}

namespace gearoenix::render::mesh {
/// TODO add instancing later
struct Builder {
protected:
    const std::shared_ptr<core::ecs::EntitySharedBuilder> entity_builder;
    const std::string name;
    const math::Aabb3 occlusion_box;

    Builder(
        core::ecs::World& world,
        const std::string& name,
        const std::vector<PbrVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        const math::Aabb3& occlusion_box) noexcept;

    void set_material(const std::type_index& material_type) noexcept;

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
        static_assert(std::is_reference_v<T>, "Material type can not be reference.");
        set_material(std::type_index(typeid(T)));
        entity_builder->get_builder().add_component(std::move(mat));
    }
};
}
#endif