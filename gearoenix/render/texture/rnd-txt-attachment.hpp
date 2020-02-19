#ifndef GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#define GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#include "../../core/cr-types.hpp"
#include "rnd-txt-face.hpp"
#include "rnd-txt-texture-info.hpp"
#include "rnd-txt-type.hpp"
#include <memory>
#include <optional>
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
    Face face = Face::PositiveZ;

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
    TextureInfo texture_info;
    unsigned int img_width = 512;
    unsigned int img_height = 512;
    unsigned int img_depth = 0;
    unsigned int mipmap_level = 0;
    UsageFlag usage = UsageFlag::Color;
    /// This field will be available whenever the attachment is cube map
    std::optional<Face> face = std::nullopt;
};
}
#endif
