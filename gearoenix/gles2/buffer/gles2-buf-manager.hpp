#ifndef GEAROENIX_GLES2_BUFFER_MANAGER_HPP
#define GEAROENIX_GLES2_BUFFER_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/buffer/rnd-buf-manager.hpp"
namespace gearoenix {
	namespace render {
		namespace buffer {
		class Uniform;
		}
	}
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace buffer {
        class Manager : public render::buffer::Manager {
        public:
            Manager(const std::shared_ptr<engine::Engine>& e);
            ~Manager();
			render::buffer::Uniform *create_uniform(const unsigned int size) override final;
        };
    }
}
}
#endif
#endif
