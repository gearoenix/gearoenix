#ifndef GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#define GEAROENIX_RENDER_TEXTURE_ATTACHMENT_HPP
#include "../../core/gx-cr-types.hpp"
#include "gx-rnd-txt-face.hpp"
#include "gx-rnd-txt-texture-info.hpp"
#include "gx-rnd-txt-type.hpp"
#include <memory>
#include <optional>
#include <variant>

namespace gearoenix::render::texture {
struct TextureCube;
struct Texture2D;

struct Attachment2D final {
    std::shared_ptr<Texture2D> txt;

    ~Attachment2D() noexcept;
};

struct AttachmentCube final {
    std::shared_ptr<TextureCube> txt;
    Face face = Face::PositiveZ;

    ~AttachmentCube() noexcept;
};

struct Attachment final {
    unsigned int mipmap_level = 0;
    std::variant<Attachment2D, AttachmentCube> var;

    ~Attachment() noexcept;
};
}
#endif
