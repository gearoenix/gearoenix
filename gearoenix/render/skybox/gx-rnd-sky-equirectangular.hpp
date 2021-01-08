#ifndef GEAROENIX_RENDER_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_RENDER_SKYBOX_EQUIRECTANGULAR_HPP
#include "gx-rnd-sky-skybox.hpp"

namespace gearoenix::render::material {
struct SkyboxEquirectangular;
}

namespace gearoenix::render::skybox {
struct Equirectangular final : public Skybox {
    // Just for reducing the unnecessary and repetitive casting
    GX_GET_CREF_PRV(std::shared_ptr<material::SkyboxEquirectangular>, mat_equ)

public:
    Equirectangular(core::Id my_id, std::string name, platform::stream::Stream* s, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Equirectangular(core::Id my_id, std::string name, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Equirectangular() noexcept final;
};
}
#endif
