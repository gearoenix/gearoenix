#ifndef GEAROENIX_RENDER_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_RENDER_SKYBOX_EQUIRECTANGULAR_HPP
#include "rnd-sky-skybox.hpp"

namespace gearoenix::render::material {
class SkyboxEquirectangular;
}

namespace gearoenix::render::skybox {
class Equirectangular final : public Skybox {
    // Just for reducing the unnecessary and repetitive casting
    GX_GET_CREF_PRV(std::shared_ptr<material::SkyboxEquirectangular>, mat_equ)

public:
    Equirectangular(core::Id my_id, std::string name, system::stream::Stream* s, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Equirectangular(core::Id my_id, std::string name, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Equirectangular() noexcept final;
};
}
#endif
