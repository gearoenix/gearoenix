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

			MathCascadeInfo(engine::Engine* e) noexcept;
			void initialize(engine::Engine* e) noexcept;
		};
		class KernelCascadeInfo {
		private:
			struct KernelRenderCascadeInfo {
				buffer::Uniform* uniform = nullptr;
				std::size_t cascade_index = 0;
				/// It is now owner of model
				model::Model* m = nullptr;

				~KernelRenderCascadeInfo() noexcept;
			};
			const std::size_t kernel_index;
			MathCascadeInfo math_info;
			std::vector<command::Buffer *> shadow_mapper_secondary_commands;
			/// Pool data
			std::vector<KernelRenderCascadeInfo> render_data_pool;
			std::size_t pool_last_index = 0;
		public:
			KernelCascadeInfo(engine::Engine* e, std::size_t kernel_index) noexcept;
			~KernelCascadeInfo() noexcept;
		};
		class FrameCascadeInfo {
		private:
			std::vector<KernelCascadeInfo *> kernels;
			MathCascadeInfo math_info;
			/// For each cascades
			std::vector<command::Buffer *> shadow_mapper_primary_commands;
			sync::Semaphore *shadow_mappers_semaphore = nullptr;
			/// Accumulate shadow
			command::Buffer *shadow_accumulator_secondary_command = nullptr;
			command::Buffer *shadow_accumulator_primary_command = nullptr;
			sync::Semaphore *shadow_accumulator_semaphore = nullptr;
		public:
			FrameCascadeInfo(engine::Engine* e) noexcept;
			~FrameCascadeInfo() noexcept;
		};
		class CascadeInfo {
		private:
			/// It is now owner of engine
			engine::Engine * e = nullptr;
			std::vector<FrameCascadeInfo *> frames;
		public:
			CascadeInfo(engine::Engine* e) noexcept;
			~CascadeInfo() noexcept;
			void set_zero_located_view(const math::Mat4x4& m) noexcept;
			void set_frustum_partitions(const std::vector<math::Vec3[4]> &p) noexcept;
		};
	}
}
#endif