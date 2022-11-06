#ifndef GEAROENIX_GL_MODEL_HPP
#define GEAROENIX_GL_MODEL_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/model/gx-rnd-mdl-builder.hpp"
#include "../render/model/gx-rnd-mdl-manager.hpp"

namespace gearoenix::gl::material {
struct Material;
}

namespace gearoenix::gl {
struct Engine;
struct Mesh;

struct Model final : public core::ecs::Component {
    GX_GET_CREF_PRV(std::shared_ptr<Mesh>, mesh);
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, mat);

public:
    Model(
        Engine& e,
        std::shared_ptr<Mesh>&& bound_mesh,
        std::shared_ptr<material::Material>&& mat,
        const std::string& name) noexcept;
    ~Model() noexcept final;
    Model(Model&&) noexcept;
};

struct ModelBuilder final : public render::model::Builder {
    friend struct ModelManager;

    Engine& e;

private:
    ModelBuilder(
        Engine& e,
        const std::string& name,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        std::shared_ptr<render::material::Material>&& mat,
        bool is_transformable) noexcept;

public:
    ~ModelBuilder() noexcept final;
};

struct ModelManager final : public render::model::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::model::Builder> build(
        std::string&& name,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        std::shared_ptr<render::material::Material>&& mat,
        const core::sync::EndCallerIgnored& c,
        bool is_transformable) noexcept final;

public:
    explicit ModelManager(Engine& e) noexcept;
    ~ModelManager() noexcept final;
};
}

#endif
#endif
