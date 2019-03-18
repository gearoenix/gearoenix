#include "rnd-pip-resource-identifier.hpp"
#include "../buffer/rnd-buf-buffer.hpp"
#include "../texture/rnd-txt-texture.hpp"

gearoenix::render::pipeline::ResourceIdentifier::ResourceIdentifier(const std::shared_ptr<buffer::Buffer> &b)
    : buffer_size(b->get_size())
{

}

gearoenix::render::pipeline::ResourceIdentifier::ResourceIdentifier(const std::shared_ptr<buffer::Buffer> &b, const std::vector<std::shared_ptr<texture::Texture>> &ts)
    : buffer_size(b->get_size())
    , textures_ids(get_textures_ids(ts))
{}

std::vector<gearoenix::core::Id> gearoenix::render::pipeline::ResourceIdentifier::get_textures_ids(const std::vector<std::shared_ptr<texture::Texture>> &ts) {
    const size_t tssize = ts.size();
    std::vector<core::Id> tis(tssize);
    for(size_t i = 0; i < tssize; ++i) tis[i] = ts[i]->get_asset_id();
    return tis;
}

bool gearoenix::render::pipeline::ResourceIdentifier::operator<(const ResourceIdentifier &o) const
{
    if (buffer_size < o.buffer_size) return true;
    const size_t textures_ids_size = textures_ids.size();
    const size_t o_textures_ids_size = o.textures_ids.size();
    if (textures_ids_size < o_textures_ids_size) return true;
    if (textures_ids_size > o_textures_ids_size) return false;
    for(size_t i = 0; i < textures_ids_size; ++i)
        if(textures_ids[i] < o.textures_ids[i]) return true;
        else if(textures_ids[i] > o.textures_ids[i]) return false;
    return false;
}
