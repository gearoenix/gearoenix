#include "gl-txt-2d.hpp"
#ifdef USE_OPENGL
#include "../../system/sys-log.hpp"

gearoenix::render::texture::Texture2D::Texture2D(system::File*, Engine*, std::shared_ptr<core::EndCaller>)
{
    TODO;
}

gearoenix::render::texture::Texture2D::~Texture2D()
{
    TODO;
}

gearoenix::render::image::View* gearoenix::render::texture::Texture2D::get_view()
{
    TODO;
    return nullptr;
}

uint32_t gearoenix::render::texture::Texture2D::get_memory_type_bits(Engine*)
{
    TODO;
    return 0;
}

#endif
