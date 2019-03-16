#ifndef GEAROENIX_GLES2_BUFFER_INDEX_HPP
#define GEAROENIX_GLES2_BUFFER_INDEX_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../render/buffer/rnd-buf-static.hpp"
#include "../gles2.hpp"
#include <vector>
namespace gearoenix {
	namespace gles2 {
		namespace engine {
			class Engine;
		}
		namespace buffer {
			class Index : public render::buffer::Static {
			public:
			private:
				GLuint bo = 0;
				GLsizei count = 0;

			public:
				Index(
					const std::vector<std::uint32_t> indices,
					const std::shared_ptr<engine::Engine>& e,
					const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
				~Index();
			};
		}
	}
}
#endif
#endif
