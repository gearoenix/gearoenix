#include "gles2-pip-forward-pbr-directional-shadow-resource-set.hpp"
#include "../shader/gles2-shd-effect-forward-pbr-directional-shadow.hpp"

gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::ForwardPbrDirectionalShadowResourceSet(const std::shared_ptr<shader::ForwardPbrDirectionalShadow>& shd)
	: ResourceSet(shd)
{
}

void gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::bind(gl::uint &bound_shader_program) const
{
	ResourceSet::bind(bound_shader_program);
	GXUNIMPLEMENTED;
}
