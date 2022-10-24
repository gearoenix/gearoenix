#ifndef GEAROENIX_GL_TARGET_HPP
#define GEAROENIX_GL_TARGET_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../core/sync/gx-cr-sync-end-caller.hpp"
#include "../render/texture/gx-rnd-txt-target.hpp"
#include "gx-gl-types.hpp"
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace gearoenix::gl {
struct Texture2D;
struct TextureCube;
struct Engine;

struct Target final : public render::texture::Target {
    struct GlAttachment final {
        std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>> texture;
        uint texture_object = static_cast<uint>(-1);
        sint mipmap_level = -1;
        enumerated target = static_cast<enumerated>(-1);
        enumerated binding_index = static_cast<enumerated>(-1);
    };

    GX_GET_RRF_PRV(Engine, e);
    GX_GET_VAL_PRV(uint, framebuffer, static_cast<uint>(-1));
    GX_GET_CREF_PRV(std::vector<GlAttachment>, gl_attachments);

public:
    Target(Engine& e, std::vector<render::texture::Attachment>&& attachments, const core::sync::EndCallerIgnored& end_callback) noexcept;
    ~Target() noexcept final;
    void bind() noexcept;
};
}

#endif
#endif