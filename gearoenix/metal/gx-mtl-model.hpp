#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-component.hpp"
#import "../render/model/gx-rnd-mdl-builder.hpp"
#import "../render/model/gx-rnd-mdl-manager.hpp"
#import "gx-mtl-buffer.hpp"

namespace gearoenix::metal {
struct Engine;
struct Mesh;
struct Model final : public core::ecs::Component {
    UniformBuffer uniform;
    ArgsBuffer gbuffers_filler_args;
    std::shared_ptr<Mesh> bound_mesh;

    Model(Engine& e, std::shared_ptr<Mesh>&& bound_mesh, NSUInteger buffer_size, const std::string& name);
    ~Model() override;
    Model(Model&&);
};

struct ModelBuilder final : public render::model::Builder {
    friend struct ModelManager;

private:
    Engine& e;

    ModelBuilder(Engine& e, const std::string& name, std::shared_ptr<render::mesh::Mesh>&& bound_mesh, bool is_transformable);

    void set_material(const render::material::Pbr& material_type) override;

public:
    ~ModelBuilder() override;
};

struct ModelManager final : public render::model::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::model::Builder> build(std::string&& name, std::shared_ptr<render::mesh::Mesh>&& mesh, core::job::EndCaller&& c, bool is_transformable) override;

public:
    explicit ModelManager(Engine& e);
    ~ModelManager() override;
};
}

#endif