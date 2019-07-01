#ifndef GEAROENIX_RENDER_PIPELINE_SHADOW_MAPPER_HPP
#define GEAROENIX_RENDER_PIPELINE_SHADOW_MAPPER_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class ShadowMapper : public Pipeline {
protected:
	ShadowMapper(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
		: Pipeline(Type::ShadowMapper, e, call) {}
public:
	virtual ~ShadowMapper() noexcept = default;
};
}
#endif
