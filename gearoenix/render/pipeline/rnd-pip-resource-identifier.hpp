#ifndef GEAROENIX_RENDER_PIPELINE_RESOURCE_IDENTIFIER_HPP
#define GEAROENIX_RENDER_PIPELINE_RESOURCE_IDENTIFIER_HPP
#include "../../core/cr-types.hpp"
#include <vector>
#include <memory>

namespace gearoenix {
namespace render {
namespace buffer
{
class Buffer;
}
namespace texture
{
class Texture;
}
namespace pipeline {
struct ResourceIdentifier
{
    const unsigned int buffer_size;
    const std::vector<core::Id> textures_ids;

    ResourceIdentifier(const std::shared_ptr<buffer::Buffer> &b);
    ResourceIdentifier(const std::shared_ptr<buffer::Buffer> &b, const std::vector<std::shared_ptr<texture::Texture>> &ts);

    static std::vector<core::Id> get_textures_ids(const std::vector<std::shared_ptr<texture::Texture>> &ts);

    bool operator < ( const ResourceIdentifier &o ) const;
};
}
}
}
#endif
