#ifndef GEAROEMIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROEMIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-cmr-uniform.hpp"

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
			class Transformation : public core::asset::Asset {
				friend class physics::Kernel;

			protected:
				Uniform uniform;
				const std::shared_ptr<buffer::FramedUniform> uniform_buffers;
				void update_location();
				void update_view_projection();
				Camera(
					const core::Id my_id,
					const std::shared_ptr<engine::Engine>& e) noexcept;
				Camera(
					const core::Id my_id,
					const std::shared_ptr<system::stream::Stream>& f,
					const std::shared_ptr<engine::Engine>& e);

			public:
				virtual ~Camera();
				void look_at(const math::Vec3& target, const math::Vec3& up);
				void look_at(const math::Vec3& origin, const math::Vec3& target, const math::Vec3& up);
				const math::Mat4x4& get_view_projection_matrix() const;
				const math::Mat4x4& get_zero_located_view() const;
				const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const;
				virtual void on_event(const core::event::Event& e);
				virtual void update_uniform();
				virtual bool in_sight(const math::Vec3& location, const core::Real radius) const = 0;
				virtual math::Ray3 create_ray3(const core::Real x, const core::Real y) const = 0;
				virtual core::Real get_distance(const math::Vec3 model_location) const = 0;
				// Transformable---------------------------------------------------------------
				void get_location(math::Vec3& l) const;
				void set_location(const math::Vec3& l);
				void translate(const math::Vec3& t);
				void global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location);
				void global_rotate(const core::Real d, const math::Vec3& axis);
				void global_rotate(const math::Mat4x4& rm);
				void local_rotate(const core::Real d, const math::Vec3& axis);
				void local_x_rotate(const core::Real d);
				void local_y_rotate(const core::Real d);
				void local_z_rotate(const core::Real d);
				void global_scale(const core::Real s);
				void local_scale(const core::Real s);
				void set_orientation(const math::Quat& q);
			};
		}
	}
}
#endif
