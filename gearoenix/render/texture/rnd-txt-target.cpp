#include "rnd-txt-target.hpp"

gearoenix::render::texture::Target::Target(const core::Id my_id, const std::shared_ptr<engine::Engine>& e)
	: Texture2D(my_id, e)
{
}

gearoenix::render::texture::Target::~Target()
{
}
