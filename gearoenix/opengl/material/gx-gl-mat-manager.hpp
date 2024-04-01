#ifndef GEAROENIX_GL_MATERIAL_MANAGER_HPP
#define GEAROENIX_GL_MATERIAL_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/material/gx-rnd-mat-manager.hpp"

namespace gearoenix::gl {
struct Engine;
struct Texture2D;
}

namespace gearoenix::gl::material {
struct Manager final : public render::material::Manager {
    Engine& gl_e;

private:
    void construct_pbr(const std::string& name, core::job::EndCallerShared<render::material::Pbr>&& c) final;
    void construct_unlit(const std::string& name, core::job::EndCallerShared<render::material::Unlit>&& c) final;
    void construct_sprite(const std::string& name, core::job::EndCallerShared<render::material::Sprite>&& c) final;

public:
    explicit Manager(Engine& e);
};
}

#endif
#endif