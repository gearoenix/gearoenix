#include "gearoenix/platform/gx-plt-file-chooser.hpp"

#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

#include <gearoenix/platform/gx-plt-file-chooser.hpp>

typedef gearoenix::core::ecs::EntityPtr GxEntityPtr;
typedef gearoenix::physics::Transformation GxTran;
typedef gearoenix::render::camera::Manager GxCamManager;
typedef gearoenix::render::light::Light GxLight;
typedef gearoenix::render::light::Manager GxLightManager;
typedef gearoenix::render::light::ShadowCasterDirectional GxShadowCasterDirLight;
typedef gearoenix::render::material::Manager GxMatManager;
typedef gearoenix::render::material::Pbr GxPbr;
typedef gearoenix::render::mesh::Manager GxMeshManager;
typedef gearoenix::render::mesh::Mesh GxMesh;
typedef gearoenix::render::model::Manager GxModelManager;
typedef gearoenix::render::scene::Manager GxSceneManager;
typedef gearoenix::render::scene::Scene GxScene;
typedef gearoenix::render::PbrVertex GxPbrVertex;

typedef std::shared_ptr<GxPbr> GxPbrPtr;
typedef std::shared_ptr<GxMesh> GxMeshPtr;

typedef gearoenix::core::job::EndCaller<> GxEndCaller;
typedef gearoenix::core::job::EndCaller<GxEntityPtr> GxEntityEndCaller;
typedef gearoenix::core::job::EndCallerShared<GxMesh> GxMeshEndCaller;
typedef gearoenix::core::job::EndCallerShared<GxPbr> GxPbrEndCaller;

struct GameApp final : gearoenix::core::Application {
private:
    GxEntityPtr scene_entity;

public:
    GameApp()
        : scene_entity(GxSceneManager::get().build("scene", 0.0))
    {
        GxMatManager::get().get_pbr("material", GxPbrEndCaller([this](GxPbrPtr&& material) -> void {
            set_material(std::move(material));
        }));
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

        GxMeshManager::get().build(
            "triangle-mesh",
            std::move(vertices),
            std::move(indices),
            std::move(material),
            GxMeshEndCaller([this](GxMeshPtr&& m) { set_mesh(std::move(m)); }));
    }

    void set_mesh(GxMeshPtr&& mesh)
    {
        auto model_builder = GxModelManager::get().build(
            "triangle-model", scene_entity.get(), { std::move(mesh) }, true);

        GxCamManager::get().build(
            "camera", scene_entity.get(),
            GxEntityEndCaller([this](GxEntityPtr&& c) { set_camera(std::move(c)); }));
    }

    void set_camera(GxEntityPtr&& camera_entity)
    {
        camera_entity->get_component<GxTran>()->set_local_position({ 0.0f, 0.0f, 5.0f });

        GxLightManager::get().build_shadow_caster_directional(
            "directional-light", scene_entity.get(), 1024, 10.0f, 1.0f, 10.0f,
            GxEntityEndCaller([this](GxEntityPtr&& l) { set_light(std::move(l)); }));
    }

    void set_light(GxEntityPtr&& light_entity)
    {
        auto* const light = light_entity->get_component<GxShadowCasterDirLight>();
        light->get_shadow_transform()->local_look_at(
            { 0.0, 0.0, 5.0 },
            { 0.0, 0.0, 0.0 },
            { 0.0, 1.0, 0.0 });
        light->colour = { 10.0f, 10.0f, 10.0f };

        scene_entity->add_to_world();

        gearoenix::platform::file_chooser_open([](auto&&, auto&&) { }, [] { }, "", "*.png");
    }
};

GEAROENIX_START(GameApp)