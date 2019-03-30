#include "gles2-pip-forward-pbr-directional-shadow-resource-set.hpp"
#include "../shader/gles2-shd-effect-forward-pbr-directional-shadow.hpp"

gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::ForwardPbrDirectionalShadowResourceSet(const std::shared_ptr<shader::ForwardPbrDirectionalShadow>& shd)
	: ResourceSet(shd)
{
}
