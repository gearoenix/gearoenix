#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

typedef gearoenix::render::material::Pbr GxPbr;
typedef std::shared_ptr<GxPbr> GxPbrPtr;
typedef gearoenix::core::job::EndCallerShared<GxPbr> GxPbrEndCaller;
typedef gearoenix::render::mesh::Mesh GxMesh;
typedef std::shared_ptr<GxMesh> GxMeshPtr;
typedef gearoenix::core::job::EndCallerShared<GxMesh> GxMeshEndCaller;
typedef gearoenix::render::camera::Builder GxCameraBuilder;
typedef std::shared_ptr<GxCameraBuilder> GxCameraBuilderPtr;
typedef gearoenix::core::job::EndCallerShared<GxCameraBuilder> GxCameraBuilderEndCaller;
typedef gearoenix::render::PbrVertex GxPbrVertex;
typedef gearoenix::core::job::EndCaller<> GxEndCaller;
typedef gearoenix::render::scene::Builder GxSceneBuilder;
typedef std::shared_ptr<GxSceneBuilder> GxSceneBuilderPtr;
typedef gearoenix::render::light::Builder GxLightBuilder;
typedef std::shared_ptr<GxLightBuilder> GxLightBuilderPtr;
typedef gearoenix::core::job::EndCallerShared<GxLightBuilder> GxLightBuilderEndCaller;

struct GameApp final : public gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        render_engine.get_material_manager()->get_pbr(
            "material",
            GxPbrEndCaller([this](GxPbrPtr&& material) mutable {
                set_material(std::move(material));
            }));
    }

    void set_mesh(GxMeshPtr&& mesh)
    {
        auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, GxEndCaller([] {}));

        auto model_builder = render_engine.get_model_manager()->build(
            "triangle-model",
            { std::move(mesh) },
            GxEndCaller([] {}),
            true);
        scene_builder->add(std::move(model_builder));

        render_engine.get_camera_manager()->build(
            "camera",
            GxCameraBuilderEndCaller([this, scene_builder = std::move(scene_builder)](GxCameraBuilderPtr&& camera_builder) mutable {
                set_camera_builder(std::move(camera_builder), std::move(scene_builder));
            }),
            GxEndCaller([] {}));
    }

    void set_material(GxPbrPtr&& material)
    {
        material->get_normal_metallic_factor().w = 0.01f;
        material->get_emission_roughness_factor().w = 0.99f;

        std::vector<GxPbrVertex> vertices(3);
        vertices[0].position = { 1.0f, -1.0f, 0.0f };
        vertices[0].normal = { 0.0f, 0.0f, 1.0f };
        vertices[0].tangent = { 0.0f, 1.0f, 0.0f, 1.0f };
        vertices[0].uv = { 1.0f, -1.0f };
        vertices[1].position = { 0.0f, 1.0f, 0.0f };
        vertices[1].normal = { 0.0f, 0.0f, 1.0f };
        vertices[1].tangent = { 0.0f, 1.0f, 0.0f, 1.0f };
        vertices[1].uv = { 0.0f, 1.0f };
        vertices[2].position = { -1.0f, -1.0f, 0.0f };
        vertices[2].normal = { 0.0f, 0.0f, 1.0f };
        vertices[2].tangent = { 0.0f, 1.0f, 0.0f, 1.0f };
        vertices[2].uv = { -1.0f, -1.0f };

        std::vector<std::uint32_t> indices = { 0, 1, 2 };

        render_engine.get_mesh_manager()->build(
            "triangle-mesh",
            std::move(vertices),
            std::move(indices),
            std::move(material),
            GxMeshEndCaller([this](GxMeshPtr&& m) {
                set_mesh(std::move(m));
            }));
    }

    void set_camera_builder(GxCameraBuilderPtr&& camera_builder, GxSceneBuilderPtr&& scene_builder)
    {
        camera_builder->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });
        scene_builder->add(std::move(camera_builder));

        render_engine.get_light_manager()->build_shadow_caster_directional(
            "directional-light",
            1024,
            10.0f,
            1.0f,
            10.0f,
            GxLightBuilderEndCaller([scene_builder = std::move(scene_builder)](GxLightBuilderPtr&& light_builder) {
                set_light_builder(std::move(light_builder), scene_builder);
            }),
            GxEndCaller([] {}));
    }

    static void set_light_builder(GxLightBuilderPtr&& light_builder, const GxSceneBuilderPtr& scene_builder)
    {
        light_builder->get_shadow_caster_directional()->get_shadow_transform()->local_look_at(
            { 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
        light_builder->get_light().colour = { 10.0f, 10.0f, 10.0f };
        scene_builder->add(std::move(light_builder));

        scene_builder->get_scene().set_enabled(true);
    }

    void update() noexcept final
    {
        Application::update();
    }
};

GEAROENIX_START(GameApp)