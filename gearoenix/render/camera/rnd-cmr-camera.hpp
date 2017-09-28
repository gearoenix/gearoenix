#ifndef GEAROEMIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROEMIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
namespace gearoenix {
namespace math {
    template <class T>
    class Vec3;
    template <class T>
    class Mat4x4;
}
namespace system {
    class File;
}
namespace render {
    namespace camera {
        class Camera : public core::asset::Asset {
        protected:
            math::Vec3<float>* loc;
            math::Mat4x4<float>* view;
            float start;
            float end;
            Camera(system::File* f);

        public:
            virtual ~Camera();
            static Camera* read(system::File* f);
        };
    }
}
}
#endif
