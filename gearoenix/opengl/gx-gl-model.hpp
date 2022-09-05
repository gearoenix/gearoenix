#ifndef GEAROENIX_GL_MODEL_HPP
#define GEAROENIX_GL_MODEL_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/material/gx-rnd-mat-pbr.hpp"
#include "../render/model/gx-rnd-mdl-builder.hpp"
#include "../render/model/gx-rnd-mdl-manager.hpp"

namespace gearoenix::gl {
struct Engine;
struct Mesh;
struct Texture2D;

struct Model final : public core::ecs::Component {
    std::shared_ptr<Mesh> bound_mesh;
    render::material::Pbr material;
    std::shared_ptr<Texture2D> albedo;
    std::shared_ptr<Texture2D> normal;
    std::shared_ptr<Texture2D> emission;
    std::shared_ptr<Texture2D> metallic_roughness;
    std::shared_ptr<Texture2D> occlusion;
    // TODO later we can bring the actual opengl objects here to make it more cache friendly

    Model(
        Engine& e,
        std::shared_ptr<Mesh>&& bound_mesh,
        render::material::Pbr&& material,
        const std::string& name) noexcept;
    ~Model() noexcept final;
    Model(Model&&) noexcept;
};

struct ModelBuilder final : public render::model::Builder {
    friend struct ModelManager;

private:
    Engine& e;

    ModelBuilder(
        Engine& e,
        const std::string& name,
        std::shared_ptr<render::mesh::Mesh>&& bound_mesh,
        bool is_transformable) noexcept;

    void set_material(const render::material::Pbr& material_type) noexcept final;

public:
    ~ModelBuilder() noexcept final;
};

struct ModelManager final : public render::model::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::model::Builder> build(
        std::string&& name,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        core::sync::EndCallerIgnored&& c,
        bool is_transformable) noexcept final;

public:
    explicit ModelManager(Engine& e) noexcept;
    ~ModelManager() noexcept final;
};
}

#endif
#endif
