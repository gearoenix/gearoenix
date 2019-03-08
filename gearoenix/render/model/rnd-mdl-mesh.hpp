#ifndef GEAROEMIX_RENDER_MODEL_MODEL_HPP
#define GEAROEMIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "../../physics/phs-transformable.hpp"
#include <map>
#include <memory>
namespace gearoenix {
namespace physics {
    namespace collider {
        class Collider;
    }
}
namespace render {
    namespace engine {
        class Engine;
    }
    class Mesh;
    namespace model {
        class Model : public core::asset::Asset {
        protected:
            const std::shared_ptr<engine::Engine> e;

            bool has_shadow_caster = false;
            bool has_transparent = false;
            math::Mat4x4 m;
            core::Real radius = 0.0;
            std::map<core::Id, std::shared_ptr<Mesh>> meshes;
            std::map<core::Id, std::shared_ptr<Model>> children;
            std::shared_ptr<physics::collider::Collider> collider = nullptr;
            Model(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);

        public:
            static Model* read_gx3d(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            virtual ~Model();
            const std::map<core::Id, std::shared_ptr<Model>>& get_children() const;
            const std::map<core::Id, std::shared_ptr<Mesh>>& get_meshes() const;
            const std::shared_ptr<physics::collider::Collider>& get_collider() const;
            const math::Mat4x4& get_model_matrix();
        };
    }
}
}
#endif
