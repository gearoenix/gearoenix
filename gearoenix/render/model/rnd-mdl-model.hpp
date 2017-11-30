#ifndef GEAROEMIX_RENDER_MODEL_MODEL_HPP
#define GEAROEMIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include <map>
#include <memory>
#include <tuple>
#include <vector>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace physics {
    class Kernel;
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

        private:
            bool has_shadow_caster = true;
            bool has_transparent = true;
            bool is_in_sun = true;
            bool is_in_camera = true;
            bool changed = true;
            // be careful about scaling because it gonna scale radius too.
            math::Mat4x4 m;
            bool needs_mvp = true;
            math::Mat4x4 mvp;
            bool needs_dbm = true;
            math::Mat4x4 dbm;
            math::Mat4x4 sunmvp;
            math::Vec3 occloc;
            // be careful about scaling because it gonna scale radius too.
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

        public:
            Model(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~Model();
            void commit(const scene::Scene* s);
            void draw(core::Id mesh_id, texture::Texture2D* shadow_texture);
            void cast_shadow(core::Id mesh_id);
            const std::map<core::Id, std::shared_ptr<Model>>& get_children() const;
            const std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& get_meshes() const;
            const math::Mat4x4& get_m() const;
            const math::Mat4x4& get_mvp() const;
            const math::Mat4x4& get_sun_mvp() const;
        };
    }
}
}
#endif
