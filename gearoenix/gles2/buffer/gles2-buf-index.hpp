#ifndef GEAROENIX_GLES2_BUFFER_INDEX_HPP
#define GEAROENIX_GLES2_BUFFER_INDEX_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/buffer/rnd-buf-static.hpp"
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
				gl::uint bo = 0;
				gl::sizei count = 0;

			public:
				Index(
					const std::vector<std::uint32_t> indices,
					const std::shared_ptr<engine::Engine>& e,
					const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
				~Index();
				void bind() const;
				void draw() const;
			};
		}
	}
}
#endif
#endif
