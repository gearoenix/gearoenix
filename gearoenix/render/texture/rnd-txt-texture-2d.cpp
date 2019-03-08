#include "rnd-txt-texture-2d.hpp"
#include "../engine/rnd-eng-engine.hpp"

gearoenix::render::texture::Texture2D::Texture2D(const core::Id my_id, const std::shared_ptr<engine::Engine>& e)
    : Texture(my_id, e)
{
}

gearoenix::render::texture::Texture2D::~Texture2D()
{
}
