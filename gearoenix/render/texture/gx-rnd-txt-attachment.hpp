#ifndef GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#define GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#include "gx-rnd-txt-face.hpp"
#include <memory>
#include <variant>

namespace gearoenix::render::texture {
struct TextureCube;
struct Texture2D;

struct Attachment2D final {
    std::shared_ptr<Texture2D> txt;

    ~Attachment2D();
};

struct AttachmentCube final {
    std::shared_ptr<TextureCube> txt;
    Face face = Face::PositiveZ;

    ~AttachmentCube();
};

struct Attachment final {
    static constexpr std::size_t ATTACHMENT_2D_VARIANT_INDEX = 0;
    static constexpr std::size_t ATTACHMENT_CUBE_VARIANT_INDEX = 1;

    unsigned int mipmap_level = 0;
    std::variant<Attachment2D, AttachmentCube> var;

    ~Attachment();
};
}
#endif
