#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-listner.hpp"
#include "../../math/math-ray.hpp"
#include "rnd-cmr-uniform.hpp"
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
    namespace collider {
        class Collider;
    }
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
        class Camera : public core::asset::Asset, public core::event::Listner {
        public:
            typedef std::array<math::Vec3, 4> Partition;

            GX_GETSET_VAL_PRT(core::Real, layer, 0.0f)
            GX_GETSET_UPTR_PRT(physics::collider::Collider, collider)
            GX_GET_UPTR_PRT(physics::Transformation, transformation)
            GX_GET_UPTR_PRT(buffer::FramedUniform, uniform_buffers)
            GX_GET_CREF_PRT(std::vector<Partition>, cascaded_shadow_frustum_partitions)
            GX_GET_CREF_PRT(Uniform, uniform)
            GX_GETSET_VAL_PRT(bool, enabled, true)
        protected:
            engine::Engine* const e;

            /// TODO change this std::shared_ptr<math::ProjectorFrustum> frustum;

            Camera(core::Id my_id, engine::Engine* e) noexcept;
            Camera(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;

        public:
            ~Camera() noexcept override;
            void set_far(core::Real f) noexcept;
            virtual void update_uniform();
            virtual void set_aspect_ratio(core::Real ratio);
            [[nodiscard]] virtual math::Ray3 create_ray3(core::Real x, core::Real y) const = 0;
            [[nodiscard]] virtual core::Real get_distance(const math::Vec3& model_location) const = 0;

            bool on_event(const core::event::Data& d) noexcept override;
        };
    }
}
}
#endif
