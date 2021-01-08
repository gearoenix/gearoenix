#ifndef GEAROENIX_RENDER_SKYBOX_CUBE_HPP
#define GEAROENIX_RENDER_SKYBOX_CUBE_HPP
#include "gx-rnd-sky-skybox.hpp"

namespace gearoenix::render::material {
struct SkyboxCube;
}

namespace gearoenix::render::skybox {
struct Cube final : public Skybox {
    // Just for reducing the unnecessary and repetitive casting
    GX_GET_CREF_PRV(std::shared_ptr<material::SkyboxCube>, mat_cube)

public:
    Cube(core::Id id, std::string name, platform::stream::Stream* s, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Cube(core::Id id, std::string name, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Cube() noexcept final;
};
}
#endif
