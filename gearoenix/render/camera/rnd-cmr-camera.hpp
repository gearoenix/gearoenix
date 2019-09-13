#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/event/cr-ev-listner.hpp"
#include "../../math/math-ray.hpp"
#include "../../core/cr-static.hpp"
#include <array>
#include <memory>
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
        class Camera : public core::asset::Asset, public core::event::Listner {
			GX_GETSET_VAL_PRT(core::Real, layer, 0.0f)
        protected:
            bool enabled = true;
            engine::Engine* const e;
            std::shared_ptr<Uniform> uniform;
            std::shared_ptr<math::ProjectorFrustum> frustum;
            std::shared_ptr<Transformation> transformation;
            std::shared_ptr<buffer::FramedUniform> uniform_buffers;
            std::shared_ptr<std::vector<std::array<math::Vec3, 4>>> cascaded_shadow_frustum_partitions;

            Camera(core::Id my_id, engine::Engine* e) noexcept;
            Camera(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;

        public:
            virtual ~Camera() noexcept;
            const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const;
            const std::shared_ptr<physics::Transformation> get_transformation() const noexcept;
			void set_far(core::Real f) noexcept;
            bool is_enabled() const noexcept;
            void enable() noexcept;
            void disable() noexcept;
            bool in_sight(const math::Vec3& location, core::Real radius) const noexcept;
            virtual void update_uniform();
            virtual void set_aspect_ratio(core::Real ratio);
            virtual math::Ray3 create_ray3(core::Real x, core::Real y) const = 0;
            virtual core::Real get_distance(const math::Vec3& model_location) const = 0;
            const std::vector<std::array<math::Vec3, 4>>& get_cascaded_shadow_frustum_partitions() noexcept;
            const Uniform& get_uniform() const noexcept;

            virtual bool on_event(const core::event::Data& d) noexcept override;
        };
    }
}
}
#endif
