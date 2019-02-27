#ifndef GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#define GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
namespace gearoenix {
	namespace render {
		class Engine;
		namespace buffer {
			class Manager {
			protected:
			public:
				Manager(Engine*) {}
				~Manager() {}
			};
		} // namespace buffer
	} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_MANAGER_HPP
