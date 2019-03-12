#include "rnd-pip-resource.hpp"

gearoenix::render::pipeline::Resource::Resource(const std::vector<std::shared_ptr<texture::Texture>>& ts)
	: ts(ts)
{
}

const std::vector<std::shared_ptr<gearoenix::render::texture::Texture>>& gearoenix::render::pipeline::Resource::get_textures() const
{
	return ts;
}
