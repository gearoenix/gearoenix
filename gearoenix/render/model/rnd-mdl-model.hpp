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
#include <mutex>
#include <tuple>
#include <vector>
namespace gearoenix {
namespace math {
    struct Ray3;
}
namespace physics {
    class Kernel;
    namespace collider {
        class Collider;
    }
}
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
    namespace material {
        class Depth;
        class Material;
    }
    namespace mesh {
        class Mesh;
    }
    namespace scene {
        class Scene;
    }
    namespace texture {
        class Texture2D;
    }
    namespace model {
        class Model : public core::asset::Asset {
        public:
            class RenderModel {
            public:
                typedef enum : core::Id {
                    DYNAMIC = 1,
                    STATIC = 2,
                    WIDGET = 3,
                } Type;
            };

        protected:
            const RenderModel::Type render_model_type;
            bool has_shadow_caster = false;
            bool has_transparent = false;
            bool is_in_sun = false;
            bool is_in_camera = false;
            // be careful about scaling because it gonna scale radius too.
            math::Mat4x4 m;
            bool needs_mvp = false;
            math::Mat4x4 mvp;
            bool needs_dbm = false;
            math::Mat4x4 dbm;
            math::Mat4x4 sunmvp;
            math::Vec3 occloc;
            // be careful about scaling because it gonna scale radius too.
            math::Vec3 occrdss;
            core::Real occrds;
            math::Vec3 moccloc;
            core::Real distcam;
            // mesh_id -> (mesh, material, shadow_caster_material)
            std::map<core::Id,
                std::tuple<std::shared_ptr<mesh::Mesh>,
                    std::shared_ptr<material::Material>,
                    std::shared_ptr<material::Depth>>>
                meshes;
            // model_id -> model
            std::map<core::Id, std::shared_ptr<Model>> children;
            std::shared_ptr<physics::collider::Collider> collider = nullptr;
            Engine* const render_engine = nullptr;
            Model(core::Id my_id, RenderModel::Type t, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);

        public:
            static Model* read(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            virtual ~Model();
            RenderModel::Type get_render_model_type() const;
            virtual void commit(const scene::Scene* s);
            void commit_camera_changes(const scene::Scene* s);
            void commit_lights_changes(const scene::Scene* s);
            void draw(core::Id mesh_id, texture::Texture2D* shadow_texture);
            void cast_shadow(core::Id mesh_id);
            const std::map<core::Id, std::shared_ptr<Model>>& get_children() const;
            const std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& get_meshes() const;
            const math::Mat4x4& get_m() const;
            const math::Mat4x4& get_mvp() const;
            const math::Mat4x4& get_sun_mvp() const;
            const std::shared_ptr<physics::collider::Collider>& get_collider() const;
            bool hit(const math::Ray3& r, core::Real& distance_from_origin);
            bool get_is_in_camera() const;
            core::Real get_distance_from_camera() const;
        };
    }
}
}
#endif
