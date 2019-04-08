#ifndef GEAROENIX_GLES2_BUFFER_MANAGER_HPP
#define GEAROENIX_GLES2_BUFFER_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/buffer/rnd-buf-manager.hpp"
namespace gearoenix {
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace buffer {
        class Manager : public render::buffer::Manager {
        public:
            Manager(const std::shared_ptr<engine::Engine>& e);
            ~Manager();
			std::shared_ptr<render::buffer::Uniform> create_uniform(const unsigned int size) override final;
			std::shared_ptr<render::buffer::Static> create_static(const std::vector<math::BasicVertex> vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) override final;
			std::shared_ptr<render::buffer::Static> create_static(const std::vector<std::uint32_t> indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) override final;
        };
    }
}
}
#endif
#endif
