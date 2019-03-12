#ifndef GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#define GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#include <memory>

namespace gearoenix {
namespace render {
	namespace engine {
		class Engine;
	}
    namespace buffer {
        class Uniform;
        class Manager {
        protected:
			const std::shared_ptr<engine::Engine> e;
        public:
			Manager(const std::shared_ptr<engine::Engine> &e);
			~Manager();
            virtual Uniform* create_uniform(const unsigned int size) = 0;
        };
    }
}
}
#endif
