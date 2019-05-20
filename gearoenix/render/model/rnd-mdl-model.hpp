#ifndef GEAROENIX_RENDER_MODEL_MODEL_HPP
#define GEAROENIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-sphere.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-mdl-uniform.hpp"
#include <map>
#include <memory>
#include <vector>
namespace gearoenix {
namespace physics::collider {
    class Collider;
}
namespace render {
    namespace buffer {
        class FramedUniform;
    }
    namespace engine {
        class Engine;
    }
    namespace model {
        class Mesh;
        class Transformation;
        class Model : public core::asset::Asset {
        protected:
            const std::shared_ptr<engine::Engine> e;
            const std::shared_ptr<buffer::FramedUniform> uniform_buffers;
            const std::shared_ptr<Transformation> transformation;

            bool is_dynamic = true;
            bool has_shadow_caster = false;
            bool has_transparent = false;
			bool enabled = true;

            Uniform uniform;
            math::Sphere occlusion_sphere = math::Sphere(math::Vec3(0.0f), 1.0f);

            std::map<core::Id, std::shared_ptr<Mesh>> meshes;
            std::map<core::Id, std::shared_ptr<Model>> children;
            std::shared_ptr<physics::collider::Collider> collider = nullptr;

        public:
            Model(
                core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            /// It will create an automatic Id for itself
            Model(
                core::Id my_id,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            virtual void update_uniform();
            void add_mesh(const std::shared_ptr<Mesh>& m);
            void add_child(const std::shared_ptr<Model>& c);
			bool is_enabled() const noexcept;
			void enable() noexcept;
			void disable() noexcept;

            const std::map<core::Id, std::shared_ptr<Model>>& get_children() const;
            const std::map<core::Id, std::shared_ptr<Mesh>>& get_meshes() const;
            const std::shared_ptr<physics::collider::Collider>& get_collider() const;
            const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const;
            const std::shared_ptr<Transformation>& get_transformation() const noexcept;
            const math::Sphere& get_occlusion_sphere() const noexcept;

            const math::Mat4x4& get_model_matrix() const;
        };
    }
}
}
#endif
