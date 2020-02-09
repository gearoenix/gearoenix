#ifndef GEAROENIX_GLC3_TEXTURE_TARGET_2D_HPP
#define GEAROENIX_GLC3_TEXTURE_TARGET_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/texture/rnd-txt-target-2d.hpp"
#include "glc3-txt-target.hpp"

namespace gearoenix::glc3::texture {
class Target2D : public render::texture::Target2D, public Target {
private:
    Target2D(core::Id id, engine::Engine* e) noexcept;

public:
    explicit Target2D(engine::Engine* e) noexcept;
    ~Target2D() noexcept final;
    [[nodiscard]] static std::shared_ptr<Target2D> construct(
        core::Id id,
        engine::Engine* e,
        const std::vector<render::texture::Info>& infos,
        unsigned int width,
        unsigned int height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
};
}

#endif
#endif
