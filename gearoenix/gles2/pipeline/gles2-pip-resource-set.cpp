#include "gles2-pip-forward-pbr-directional-shadow-resource-set.hpp"
#include "gles2-pip-resource-set.hpp"

gearoenix::gles2::pipeline::ResourceSet::ResourceSet(const std::shared_ptr<shader::Shader>& shd)
	: shd(shd)
{
}
