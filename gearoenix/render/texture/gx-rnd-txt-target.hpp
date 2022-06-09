#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-2d.hpp"
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
    Target(std::vector<Attachment>&& attachments) noexcept;

public:
    virtual ~Target() noexcept;
    [[nodiscard]] math::Vec2<std::size_t> get_dimension() const noexcept;
};
}
#endif
