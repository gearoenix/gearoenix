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
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_CVAL_PRT(core::Id, target_id)
    GX_GETSET_VAL_PRT(unsigned int, clipping_width, 0)
    GX_GETSET_VAL_PRT(unsigned int, clipping_height, 0)
    GX_GETSET_VAL_PRT(unsigned int, clipping_starting_x, 0)
    GX_GETSET_VAL_PRT(unsigned int, clipping_starting_y, 0)
    GX_GET_CREF_PRT(std::vector<Attachment>, attachments)
    GX_GETSET_VAL_PRT(bool, depth_test_enabled, true)
    GX_GETSET_VAL_PRT(bool, scissor_test_enabled, true)
    GX_GETSET_VAL_PRT(bool, stencil_test_enabled, true)
    GX_GETSET_VAL_PRT(bool, write_depth, true)
    GX_GETSET_CREF_PRT(math::Vec4<float>, clear_color)
protected:
    Target(const core::Id id, engine::Engine* const e) noexcept
        : e(e)
        , target_id(id)
    {
    }

    Target(const Target& o) noexcept
        : e(o.e)
        , target_id(o.target_id)
        , clipping_width(o.clipping_width)
        , clipping_height(o.clipping_height)
        , clipping_starting_x(o.clipping_starting_x)
        , clipping_starting_y(o.clipping_starting_y)
        , attachments(o.attachments)
        , depth_test_enabled(o.depth_test_enabled)
        , scissor_test_enabled(o.scissor_test_enabled)
        , stencil_test_enabled(o.stencil_test_enabled)
        , write_depth(o.write_depth)
    {
    }

public:
    virtual ~Target() noexcept = default;

    [[nodiscard]] const std::shared_ptr<Texture>& get_texture(std::size_t index) const noexcept
    {
        return attachments[index].txt;
    }

    [[nodiscard]] std::size_t get_textures_count() const noexcept
    {
        return attachments.size();
    }

    [[nodiscard]] virtual Target* clone() const noexcept = 0;
};
}
#endif
