#ifndef GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#define GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "../../math/math-aabb.hpp"
#include <vector>
#include <memory>
namespace gearoenix::render {
	namespace buffer {
		class Uniform;
	}
	namespace command { 
		class Buffer; 
	}
	namespace engine {
		class Engine;
	}
	namespace model {
		class Model;
	}
	namespace sync {
		class Semaphore;
	}
	namespace light {
		struct CameraCascadeInfo {
			math::Mat4x4 vp;
			math::Aabb3 boundary;
			math::Aabb3 seen;
		};
		struct MathCascadeInfo {
			math::Mat4x4 zero_located_view;
			std::vector<CameraCascadeInfo> cameras;
		};
		struct KernelRenderCascadeInfo {
			std::shared_ptr<buffer::Uniform> uniform = nullptr;
			std::size_t cascade_index;
			model::Model* model = nullptr;
		};
		struct KernelCascadeInfo {
			MathCascadeInfo math_info;
			std::vector<std::shared_ptr<command::Buffer>> shadow_mapper_secondary_commands;
			/// Pool data
			std::vector<KernelRenderCascadeInfo> render_data_pool;
			std::size_t pool_last_index = 0;
		};
		struct FrameCascadeInfo {
			std::vector<KernelCascadeInfo> kernels;
			MathCascadeInfo math_info;
			/// For each cascades
			std::vector<std::shared_ptr<command::Buffer>> shadow_mapper_primary_commands;
			std::shared_ptr<sync::Semaphore> shadow_mappers_semaphore = nullptr;
			/// Accumulate shadow
			std::shared_ptr<command::Buffer> shadow_accumulator_secondary_command = nullptr;
			std::shared_ptr<command::Buffer> shadow_accumulator_primary_command = nullptr;
			std::shared_ptr<sync::Semaphore> shadow_accumulator_semaphore = nullptr;
		
			void initialize(const std::shared_ptr<engine::Engine>& e) noexcept;
		};
		class CascadeInfo {
		private:
			const std::shared_ptr<engine::Engine> e;
			std::vector<FrameCascadeInfo> frames;
		public:
			CascadeInfo(const std::shared_ptr<engine::Engine>& e) noexcept;
			void set_zero_located_view(const math::Mat4x4& m) noexcept;
			void set_frustum_partitions(const std::vector<math::Vec3[4]> &p) noexcept;
		};
	}
}
#endif