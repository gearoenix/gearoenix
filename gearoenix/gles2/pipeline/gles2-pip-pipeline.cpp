#include "gles2-pip-pipeline.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "../shader/gles2-shader.hpp"

gearoenix::gles2::pipeline::Pipeline::Pipeline(
	const render::pipeline::Type::Id pid,
	const std::shared_ptr<engine::Engine> &e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> call)
    : render::pipeline::Pipeline(pid, eng, call)
{
}

gearoenix::gles2::pipeline::Pipeline::~Pipeline()
{
}

void gearoenix::gles2::pipeline::Pipeline::bind()
{
    GXUNIMPLEMENTED;
}

#endif
