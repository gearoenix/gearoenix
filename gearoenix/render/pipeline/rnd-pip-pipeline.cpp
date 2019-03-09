#include "rnd-pip-pipeline.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/rnd-eng-engine.hpp"

gearoenix::render::pipeline::Pipeline::Pipeline(const Type::Id pipeline_type, const std::shared_ptr<engine::Engine> e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &call)
    : e(e)
	, pipeline_type(pipeline_type)
{
}

gearoenix::render::pipeline::Type::Id gearoenix::render::pipeline::Pipeline::get_pipeline_type_id() const
{
    return pipeline_type;
}

gearoenix::render::pipeline::Pipeline::~Pipeline()
{
}