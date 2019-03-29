#ifndef GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#define GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../render/pipeline/rnd-pip-manager.hpp"
namespace gearoenix {
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace pipeline {
		class Pipeline;
        class Manager : public render::pipeline::Manager {
		private:
			core::cache::Cacher<Pipeline> cacheds;
        public:
            Manager(const std::shared_ptr<engine::Engine>& engine);
            ~Manager() override final;
            std::shared_ptr<render::pipeline::Pipeline> get(const render::pipeline::Type::Id pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline> &end) override final;
        };
    }
}
}
#endif
