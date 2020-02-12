#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_2D_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_2D_HPP
#include "rnd-txt-target.hpp"
#include "rnd-txt-texture-2d.hpp"

namespace gearoenix::render::texture {
class Target2D : public Target {
protected:
    Target2D(const core::Id id, engine::Engine* const e) noexcept
        : Target(Type::Target2D, id, e)
    {
    }

public:
    ~Target2D() noexcept override = default;

    [[nodiscard]] std::shared_ptr<Texture> get_texture(const std::size_t index) const noexcept override
    {
        return get_texture_2d(index);
    }

    [[nodiscard]] virtual std::shared_ptr<Texture2D> get_texture_2d(std::size_t index) const noexcept = 0;
};
}
#endif
