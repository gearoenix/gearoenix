#ifndef GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#define GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#include "../../render/pipeline/rnd-pip-manager.hpp"
namespace gearoenix {
namespace gles2 {
	namespace engine {
		class Engine;
	}
    namespace pipeline {
        class Manager : public render::pipeline::Manager {
        public:
            Manager(const std::shared_ptr<engine::Engine> &engine);
            ~Manager() override final;
            std::shared_ptr<render::pipeline::Pipeline> get(const render::pipeline::Type::Id pipeline_type_id, core::sync::EndCaller<core::sync::EndCallerIgnore> end) override final;
            std::shared_ptr<render::pipeline::Resource> create_resource(const std::vector<std::shared_ptr<render::texture::Texture>>& ts) override final;
        };
    }
}
}
#endif // GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
