#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-txt-attachment.hpp"
#include "gx-rnd-txt-type.hpp"
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Target {
    GX_GET_CREF_PRT(std::vector<Attachment>, attachments)

protected:
    Target(std::vector<Attachment>&& attachments) noexcept
        : attachments(std::move(attachments))
    {
    }

public:
    virtual ~Target() noexcept = default;
};
}
#endif
