#ifndef GEAROENIX_GL_TARGET_HPP
#define GEAROENIX_GL_TARGET_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-gl-types.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace gearoenix::gl {
struct Texture2D;
struct Target final {
    struct Attachment final {
        std::shared_ptr<Texture2D> texture;
        uint mipmap_level = 0;
        enumerated binding_index = static_cast<enumerated>(-1);
    };

    GX_GET_VAL_PRV(uint, framebuffer, static_cast<uint>(-1))
    GX_GET_CREF_PRV(std::vector<Attachment>, attachments)
    GX_GET_CREF_PRV(std::vector<Attachment>, colour_attachments)
    GX_GET_CREF_PRV(std::optional<Attachment>, depth_attachment)
    GX_GET_CREF_PRV(std::optional<uint>, depth_render_buffer)

    Target(const std::vector<Attachment>& attachments, bool is_depth_necessary = true) noexcept;
    ~Target() noexcept;
    void bind() noexcept;
};
}

#endif
#endif