#ifndef GEAROENIX_GL_MODEL_HPP
#define GEAROENIX_GL_MODEL_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/model/gx-rnd-mdl-builder.hpp"
#include "../render/model/gx-rnd-mdl-manager.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"

namespace gearoenix::gl::material {
struct Material;
}

namespace gearoenix::gl {
struct Engine;
struct Mesh;

struct Model final : render::model::Model {
    GX_GET_CREF_PRV(std::shared_ptr<Mesh>, gl_mesh);
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, gl_material);

    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const override;

public:
    Model(
        Engine& e,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        std::shared_ptr<render::material::Material>&& mat,
        std::string&& name,
        bool is_transformable);
    [[nodiscard]] static std::shared_ptr<Model> construct(
        Engine& e,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        std::shared_ptr<render::material::Material>&& mat,
        std::string&& name,
        bool is_transformable);
    ~Model() override;
};

struct ModelBuilder final : render::model::Builder {
    friend struct ModelManager;

    Engine& e;

private:
    ModelBuilder(
        Engine& e,
        std::string&& name,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        std::shared_ptr<render::material::Material>&& mat,
        core::job::EndCaller<>&& end_caller,
        bool is_transformable);

public:
    ~ModelBuilder() override;
};

struct ModelManager final : render::model::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::model::Builder> build(
        std::string&& name,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        std::shared_ptr<render::material::Material>&& mat,
        core::job::EndCaller<>&& c,
        bool is_transformable) override;

public:
    explicit ModelManager(Engine& e);
    ~ModelManager() override;
};
}

#endif
#endif
