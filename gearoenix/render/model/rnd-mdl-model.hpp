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
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
    namespace material {
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
        private:
            math::Mat4x4 m;
            math::Mat4x4 mvp;
            std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>>> meshes;
            std::map<core::Id, std::shared_ptr<Model>> children;

        public:
            Model(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
            ~Model();
            void commit(const scene::Scene* s);
            void draw(texture::Texture2D* shadow_texture);
            const std::map<core::Id, std::shared_ptr<Model>>& get_children() const;
            const std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>>>& get_meshes() const;
            const math::Mat4x4& get_m() const;
            const math::Mat4x4& get_mvp() const;
        };
    }
}
}
#endif
