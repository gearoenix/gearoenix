#ifndef GEAROENIX_RENDER_BUFFER_FRAMED_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_FRAMED_UNIFORM_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <vector>
#include <memory>
namespace gearoenix {
namespace render {
	namespace engine {
		class Engine;
	}
    namespace buffer {
		class Uniform;
        class FramedUniform {
        protected:
			const std::shared_ptr<engine::Engine> e;
			const std::vector<std::shared_ptr<Uniform>> uniforms;
        public:
            FramedUniform(const unsigned int s, const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            ~FramedUniform();
			// Update current frame uniform buffer
            void update(const void* data);
			// return current frame uniform buffer
            const std::shared_ptr<Uniform> &get_buffer() const;
        };
    }
}
}
#endif