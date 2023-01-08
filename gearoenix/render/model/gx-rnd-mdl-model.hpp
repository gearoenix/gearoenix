#ifndef GEAROENIX_RENDER_MODEL_MODEL_HPP
#define GEAROENIX_RENDER_MODEL_MODEL_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../texture/gx-rnd-txt-binding-point.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <map>
#include <vector>

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Model final : public core::ecs::Component {
    GX_GET_VAL_PRV(bool, is_transformable, false);
    GX_GET_CREF_PRV(std::shared_ptr<mesh::Mesh>, bound_mesh);
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, bound_material);

public:
    std::optional<boost::container::flat_set<core::ecs::entity_id_t>> cameras;
    std::uint64_t cameras_flags = static_cast<std::uint64_t>(-1);
    core::ecs::entity_id_t scene_id = 0;

    ///
    /// \param is_transformable It indicates whether the model is not static or dynamic.
    /// \param bound_mesh
    /// \param bound_material
    /// \note A model can be static while it has transform component.
    Model(
        bool is_transformable,
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        std::shared_ptr<material::Material>&& bound_material) noexcept;
    ~Model() noexcept final;
    Model(Model&&) noexcept;
};
}
#endif
