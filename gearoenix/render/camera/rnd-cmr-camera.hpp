#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-ray.hpp"
#include <vector>

namespace gearoenix {
namespace core::event {
    class Event;
}
namespace math {
    struct ProjectorFrustum;
    struct Quat;
}
namespace physics {
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
            bool enabled = true;
            const std::shared_ptr<engine::Engine> e;
            std::shared_ptr<Uniform> uniform;
            std::shared_ptr<math::ProjectorFrustum> frustum;
            std::shared_ptr<Transformation> transformation;
            std::shared_ptr<buffer::FramedUniform> uniform_buffers;

            Camera(
                core::Id my_id,
                const std::shared_ptr<engine::Engine>& e) noexcept;
            Camera(
                core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e);

        public:
            const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const;
            const std::shared_ptr<physics::Transformation> get_transformation() const noexcept;
            bool is_enabled() const noexcept;
            void enable() noexcept;
            void disable() noexcept;
            bool in_sight(const math::Vec3& location, core::Real radius) const noexcept;
            virtual void update_uniform();
            virtual void set_aspect_ratio(core::Real ratio);
            virtual math::Ray3 create_ray3(core::Real x, core::Real y) const = 0;
            virtual core::Real get_distance(const math::Vec3& model_location) const = 0;
            virtual std::vector<math::Vec3[4]> get_cascaded_shadow_frustum_partitions() const noexcept = 0;
        };
    }
}
}
#endif
