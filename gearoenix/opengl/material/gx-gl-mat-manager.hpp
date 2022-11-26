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
    [[nodiscard]] std::shared_ptr<render::material::Pbr> construct_pbr(const std::string& name, const core::sync::EndCaller& c) noexcept final;
    [[nodiscard]] std::shared_ptr<render::material::Unlit> construct_unlit(const std::string& name, const core::sync::EndCaller& c) noexcept final;
    [[nodiscard]] std::shared_ptr<render::material::Sprite> construct_sprite(const std::string& name, const core::sync::EndCaller& c) noexcept final;

public:
    explicit Manager(Engine& e) noexcept;
};
}

#endif
#endif