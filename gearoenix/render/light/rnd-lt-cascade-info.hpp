#ifndef GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#define GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-lt-light.hpp"
namespace gearoenix::render {
	namespace command { 
		class Buffer; 
	}
	namespace sync {
		class Semaphore;
	}
	namespace light {
		struct KernelCascadeInfo {

		};
		struct FrameCascadeInfo {
			std::vector<KernelCascadeInfo> kernels;
			/// For each cascades
			std::vector<std::shared_ptr<command::Buffer>> shadow_mapper_primary_commands;
			std::shared_ptr<sync::Semaphore> shadow_mappers_semaphore;
			/// Accumulate shadow
			std::shared_ptr<command::Buffer> shadow_accumulator_secondary_command;
			std::shared_ptr<command::Buffer> shadow_accumulator_primary_command;
			std::shared_ptr<sync::Semaphore> shadow_accumulator_semaphore;
		};
		class CascadeInfo {
		private:
			std::vector<FrameCascadeInfo> frames;
		public:

		};
	}
}
#endif