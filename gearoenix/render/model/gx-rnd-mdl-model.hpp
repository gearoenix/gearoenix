#ifndef GEAROENIX_RENDER_MODEL_MODEL_HPP
#define GEAROENIX_RENDER_MODEL_MODEL_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include <boost/container/flat_set.hpp>

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Model : core::ecs::Component {
    static constexpr std::size_t MAX_COUNT = 1024;

    GX_GET_VAL_PRT(bool, is_transformable, false);
    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, bound_mesh);
    GX_GET_CREF_PRT(std::shared_ptr<material::Material>, bound_material);

    /// \note A model can be static while it has transform component.
    Model(
        std::type_index final_component_type,
        bool is_transformable,
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        std::shared_ptr<material::Material>&& bound_material,
        std::string&& name);

public:
    std::optional<boost::container::flat_set<core::ecs::entity_id_t>> cameras;
    std::uint64_t cameras_flags = static_cast<std::uint64_t>(-1);
    core::ecs::entity_id_t scene_id = 0;

    ~Model() override;
};
}
#endif
