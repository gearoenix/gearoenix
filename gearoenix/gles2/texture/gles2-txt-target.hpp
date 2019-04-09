#ifndef GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#define GEAROENIX_GLES2_TEXTURE_TARGET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-target.hpp"
#include "../../render/texture/rnd-txt-format.hpp"
#include "../../render/texture/rnd-txt-sample.hpp"
#include "../gles2.hpp"
#include <memory>
namespace gearoenix {
	namespace gles2 {
		namespace engine {
			class Engine;
		}
		namespace texture {
			class Target : public render::texture::Target {
			private:
				gl::uint texture_object = 0;
				gl::sint framebuffer = 0;
				gl::sint depth_buffer = 0;
			public:
				Target(const std::shared_ptr<engine::Engine>& e);
				~Target() override final;
				void bind() const;
			};
		}
	}
}
#endif
#endif
