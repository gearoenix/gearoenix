#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-ray.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        class Event;
    }
}
namespace math {
    struct Quat;
}
namespace physics {
    class Kernel;
    class Transformation;
}
namespace system {
    class Application;
    class File;
}
namespace render {
    namespace buffer {
        class FramedUniform;
    }
    namespace engine {
        class Engine;
    }
    namespace camera {
        class Transformation;
        struct Uniform;
        class Camera : public core::asset::Asset {
        protected:
            std::shared_ptr<Uniform> uniform;
            std::shared_ptr<Transformation> transformation;
            std::shared_ptr<buffer::FramedUniform> uniform_buffers;

            Camera(
                const core::Id my_id,
                const std::shared_ptr<engine::Engine>& e) noexcept;
            Camera(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Camera();
            virtual void update_uniform();
            const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const;
            const std::shared_ptr<physics::Transformation> get_transformation() const noexcept;
            virtual bool in_sight(const math::Vec3& location, const core::Real radius) const = 0;
            virtual math::Ray3 create_ray3(const core::Real x, const core::Real y) const = 0;
            virtual core::Real get_distance(const math::Vec3& model_location) const = 0;
        };
    }
}
}
#endif
