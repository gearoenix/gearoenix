#ifndef GEAROEMIX_RENDER_MODEL_MODEL_HPP
#define GEAROEMIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include <map>
#include <memory>
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
namespace system {
    class File;
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
        class Uniform;
        class Model : public core::asset::Asset {
            friend class physics::Kernel;

        public:
            typedef enum : core::Id {
                BASIC = 1,
                WIDGET = 2,
                UNKNOWN = 0XFFFFFFFFFFFFFFFF,
            } ModelType;

        protected:
            const ModelType model_type;
            bool has_shadow_caster = false;
            bool has_transparent = false;
            bool is_in_sun = false;
            bool is_in_camera = false;
            bool changed = true;
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
            core::Real distcam;
            // mesh_id -> (mesh, material, shadow_caster_material)
            std::map<core::Id,
                std::tuple<std::shared_ptr<mesh::Mesh>,
                    std::shared_ptr<material::Material>,
                    std::shared_ptr<material::Depth>>>
                meshes;
            // model_id -> model
            std::map<core::Id, std::shared_ptr<Model>> children;
            physics::collider::Collider* collider = nullptr;
            std::vector<math::Mat4x4> state;
            Model(ModelType t, system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);

        public:
            static Model* read(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~Model();
            void commit(const scene::Scene* s);
            void draw(core::Id mesh_id, texture::Texture2D* shadow_texture);
            void cast_shadow(core::Id mesh_id);
            const std::map<core::Id, std::shared_ptr<Model>>& get_children() const;
            const std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& get_meshes() const;
            const math::Mat4x4& get_m() const;
            const math::Mat4x4& get_mvp() const;
            const math::Mat4x4& get_sun_mvp() const;
            void translate(const math::Vec3& t);
            void global_scale(const core::Real s);
            void local_scale(const core::Real s);
            ModelType get_type() const;
            bool hit(const math::Ray3& r, core::Real& distance_from_origin) const;
            const physics::collider::Collider* get_collider() const;
            void push_state();
            void pop_state();
        };
    }
}
}
#endif
