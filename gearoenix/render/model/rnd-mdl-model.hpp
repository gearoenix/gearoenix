#ifndef GEAROEMIX_RENDER_MODEL_MODEL_HPP
#define GEAROEMIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include <memory>
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
    namespace scene {
        class Scene;
    }
    namespace model {
        class Uniform;
        class Model : public core::asset::Asset {
        protected:
            math::Mat4x4 m;
            math::Mat4x4 mvp;
            Model();
            static Model* read_child(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);

        public:
            virtual ~Model();
            virtual void commit(const scene::Scene* s) = 0;
            virtual void commit(const scene::Scene* s, const Model* parent) = 0;
            virtual void draw() = 0;
            const math::Mat4x4& get_m() const;
            const math::Mat4x4& get_mvp() const;
            static Model* read(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
        };
    }
}
}
#endif
