#ifndef GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#define GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#include "../../core/cr-types.hpp"
#include "rnd-txt-face.hpp"
#include "rnd-txt-type.hpp"
#include <memory>
#include <variant>

namespace gearoenix::render::texture {
enum struct UsageFlag : core::TypeId {
    Depth = 1,
    Color = 2,
    Stencil = 4,
};

class Texture;
class TextureCube;
class Texture2D;

struct Attachment2D {
    std::shared_ptr<Texture2D> txt;

    ~Attachment2D() noexcept;
};

struct AttachmentCube {
    std::shared_ptr<TextureCube> txt;
    Face face;

    ~AttachmentCube() noexcept;
};

struct Attachment {
    std::shared_ptr<Texture> txt = nullptr;
    unsigned int img_width = 0;
    unsigned int img_height = 0;
    unsigned int img_depth = 0;
    unsigned int mipmap_level = 0;
    UsageFlag usage = UsageFlag::Color;
    std::variant<Attachment2D, AttachmentCube> var;

    ~Attachment() noexcept;
};

struct AttachmentInfo {
    Type img_type = Type::Texture2D;
    unsigned int img_width = 512;
    unsigned int img_height = 512;
    unsigned int img_depth = 0;
    unsigned int mipmap_level = 0;
    UsageFlag usage = UsageFlag::Color;
};
}
#endif
