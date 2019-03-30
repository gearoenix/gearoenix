#include "gles2-pip-forward-pbr-directional-shadow.hpp"
#include "../shader/gles2-shd-effect-forward-pbr-directional-shadow.hpp"
#include "gles2-pip-forward-pbr-directional-shadow-resource-set.hpp"

gearoenix::gles2::pipeline::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: Pipeline(render::pipeline::Type::ForwardPbrDirectionalShadow, e, c)
	, shd(new shader::ForwardPbrDirectionalShadow(e, c))
{}

gearoenix::render::pipeline::ResourceSet * gearoenix::gles2::pipeline::ForwardPbrDirectionalShadow::create_resource_set() const
{
	return new ForwardPbrDirectionalShadowResourceSet(shd);
}
