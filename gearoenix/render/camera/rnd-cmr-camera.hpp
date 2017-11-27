#ifndef GEAROEMIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROEMIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
namespace gearoenix {
namespace physics {
    class Kernel;
}
namespace system {
    class Application;
    class File;
}
namespace render {
    namespace camera {
        class Camera : public core::asset::Asset {
            friend class physics::Kernel;

        protected:
            bool moved = true;
            core::Real start;
            core::Real end;
            core::Real screen_ratio;
            core::Real c_width;
            core::Real c_height;
            math::Vec3 x;
            math::Vec3 y;
            math::Vec3 z;
            math::Vec3 l;
            math::Mat4x4 vwl;
            math::Mat4x4 v;
            math::Mat4x4 p;
            math::Mat4x4 vp;
            Camera(system::File* f, system::Application* sysapp);

        public:
            virtual ~Camera();
            static Camera* read(system::File* f, system::Application* sysapp);
            void translate(const math::Vec3& vec);
            void move(const math::Vec3& vec);
            void move_forward(const core::Real spd);
            void move_sideward(const core::Real spd);
            void rotate_local_x(const core::Real rad);
            void rotate_local_y(const core::Real rad);
            void rotate_local_z(const core::Real rad);
            void rotate_local(const core::Real rad, const math::Vec3& vec);
            void rotate_global_x(const core::Real rad);
            void rotate_global_y(const core::Real rad);
            void rotate_global_z(const core::Real rad);
            void rotate_global(const core::Real rad, const math::Vec3& vec);
            void rotate_look_at(const core::Real rad, const math::Vec3& vec, const math::Vec3& p);
            const math::Mat4x4& get_view_projection() const;
            const math::Mat4x4& get_zero_located_view() const;
            const math::Vec3& get_location() const;
            const math::Vec3& get_x() const;
            const math::Vec3& get_y() const;
            const math::Vec3& get_z() const;
            void copy_location(math::Vec3& v) const;
            virtual bool in_sight(const math::Vec3& location, const core::Real radius) = 0;
            virtual void window_size_changed() = 0;
        };
    }
}
}
#endif
