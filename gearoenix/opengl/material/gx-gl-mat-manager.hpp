#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/material/gx-rnd-mat-manager.hpp"

namespace gearoenix::gl {
struct Texture2D;
}

namespace gearoenix::gl::material {
struct Manager final : render::material::Manager {

private:
    void construct_pbr(const std::string& name, core::job::EndCallerShared<render::material::Pbr>&& c) override;
    void construct_unlit(const std::string& name, core::job::EndCallerShared<render::material::Unlit>&& c) override;
    void construct_sprite(const std::string& name, core::job::EndCallerShared<render::material::Sprite>&& c) override;

public:
    Manager();
};
}

#endif