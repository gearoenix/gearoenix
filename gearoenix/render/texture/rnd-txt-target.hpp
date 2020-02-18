#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "../../core/cr-static.hpp"
#include "rnd-txt-attachment.hpp"
#include "rnd-txt-type.hpp"
#include <vector>

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::texture {
class Target {
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_CVAL_PRT(core::Id, target_id)
    GX_GETSET_VAL_PRT(unsigned int, clipping_width, 0)
    GX_GETSET_VAL_PRT(unsigned int, clipping_height, 0)
    GX_GETSET_VAL_PRT(unsigned int, clipping_starting_x, 0)
    GX_GETSET_VAL_PRT(unsigned int, clipping_starting_y, 0)
    GX_GET_CREF_PRT(std::vector<Attachment>, attachents)
    GX_GETSET_VAL_PRT(bool, depth_test_enabled, true)
    GX_GETSET_VAL_PRT(bool, scissor_test_enabled, true)
    GX_GETSET_VAL_PRT(bool, stencil_test_enabled, true)
    GX_GETSET_VAL_PRT(bool, write_depth, true)
protected:
    Target(const core::Id id, engine::Engine* const e) noexcept
        : e(e)
        , target_id(id)
    {
    }

public:
    virtual ~Target() noexcept = default;
    [[nodiscard]] virtual std::shared_ptr<Texture> get_texture(std::size_t index) const noexcept = 0;
    [[nodiscard]] virtual std::size_t get_textures_count() const noexcept = 0;
    [[nodiscard]] virtual Target* clone() const noexcept = 0;
};
}
#endif
