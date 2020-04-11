#ifndef GEAROENIX_RENDER_SKYBOX_CUBE_HPP
#define GEAROENIX_RENDER_SKYBOX_CUBE_HPP
#include "rnd-sky-skybox.hpp"

namespace gearoenix::render::material {
class SkyboxCube;
}

namespace gearoenix::render::skybox {
class Cube final : public Skybox {
    // Just for reducing the unnecessary and repetitive casting
    GX_GET_CREF_PRV(std::shared_ptr<material::SkyboxCube>, mat_cube)

public:
    Cube(core::Id my_id, system::stream::Stream* s, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Cube(core::Id my_id, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Cube() noexcept final;
};
}
#endif
