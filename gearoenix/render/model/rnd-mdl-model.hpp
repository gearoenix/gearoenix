#ifndef GEAROEMIX_RENDER_MODEL_MODEL_HPP
#define GEAROEMIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
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
    namespace model {
        class Uniform;
        class Model : public core::asset::Asset {
        protected:
            Model();
            static Model* read_child(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);

        public:
            virtual ~Model();
            virtual void draw(const std::shared_ptr<camera::Camera>& cam) = 0;
            virtual void draw(const std::shared_ptr<camera::Camera>& cam, const Uniform& pu) = 0;
            static Model* read(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c);
        };
    }
}
}
#endif
