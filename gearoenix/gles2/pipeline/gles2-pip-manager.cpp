#include "gles2-pip-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gles2-eng-engine.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "gles2-pip-forward-pbr-directional-shadow.hpp"

gearoenix::gles2::pipeline::Manager::Manager(const std::shared_ptr<engine::Engine>& engine)
    : render::pipeline::Manager(engine)
{
}

gearoenix::gles2::pipeline::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::gles2::pipeline::Manager::get(const render::pipeline::Type::Id pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline> &end)
{
	const std::shared_ptr<render::pipeline::Pipeline> p = pipelines.get<render::pipeline::Pipeline>(pipeline_type_id, [this, pipeline_type_id, end] {
		const std::shared_ptr<engine::Engine> gles2eng = std::static_pointer_cast<engine::Engine>(e);
		switch (pipeline_type_id)
		{
		case render::pipeline::Type::ForwardPbrDirectionalShadow:
			return std::shared_ptr<render::pipeline::Pipeline>(new ForwardPbrDirectionalShadow(gles2eng, 
				core::sync::EndCaller<core::sync::EndCallerIgnore>([end] {})));
		default:
			GXLOGF("Unexpected pipeline type: " << pipeline_type_id);
			break;
		}
	});
	end.set_data(p);
	return p;
}

#endif
