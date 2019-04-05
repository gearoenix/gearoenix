#include "rnd-pip-forward-pbr-directional-shadow-resource-set.hpp"

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_node_uniform_buffer(const std::shared_ptr<buffer::Uniform>& uniform_buffer)
{
	node_uniform_buffer = uniform_buffer;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_diffuse_environment(const std::shared_ptr<texture::Cube>& t)
{
	diffuse_environment = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_specular_environment(const std::shared_ptr<texture::Cube>& t)
{
	specular_environment = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t)
{
	ambient_occlusion = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t)
{
	shadow_mapper = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_brdflut(const std::shared_ptr<texture::Texture2D>& t)
{
	brdflut = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::clean()
{
	ResourceSet::clean();
	diffuse_environment = nullptr;
	specular_environment = nullptr;
	ambient_occlusion = nullptr;
	shadow_mapper = nullptr;
	brdflut = nullptr;
}
