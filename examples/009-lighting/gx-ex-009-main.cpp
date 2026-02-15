#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-runtime.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

using GxCoreApp = gearoenix::core::Application;
using GxEntityPtr = gearoenix::core::ecs::EntityPtr;
using GxEndCaller = gearoenix::core::job::EndCaller<>;
using GxEntityEndCaller = gearoenix::core::job::EndCaller<GxEntityPtr>;
using GxAabb3 = gearoenix::math::Aabb3<double>;
using GxVec3 = gearoenix::math::Vec3<double>;
using GxConstraintManager = gearoenix::physics::constraint::Manager;
using GxTransform = gearoenix::physics::Transformation;
using GxPath = gearoenix::platform::stream::Path;
using GxStream = gearoenix::platform::stream::Stream;
using GxLocal = gearoenix::platform::stream::Local;
using GxCameraManager = gearoenix::render::camera::Manager;
using GxLightManager = gearoenix::render::light::Manager;
using GxShadowCaster = gearoenix::render::light::ShadowCasterDirectional;
using GxMaterialManager = gearoenix::render::material::Manager;
using GxPbr = gearoenix::render::material::Pbr;
using GxMeshManager = gearoenix::render::mesh::Manager;
using GxMesh = gearoenix::render::mesh::Mesh;
using GxModelManager = gearoenix::render::model::Manager;
using GxReflectionManager = gearoenix::render::reflection::Manager;
using GxReflectionRuntime = gearoenix::render::reflection::Runtime;
using GxSceneManager = gearoenix::render::scene::Manager;
using GxSkyboxManager = gearoenix::render::skybox::Manager;

using GxMeshEndCaller = gearoenix::core::job::EndCallerShared<GxMesh>;
using GxPbrEndCaller = gearoenix::core::job::EndCallerShared<GxPbr>;
using GxMeshPtr = std::shared_ptr<GxMesh>;
using GxPbrPtr = std::shared_ptr<GxPbr>;

struct GameApp final : GxCoreApp {
private:
    GxEntityPtr scene_entity;

public:
    GameApp()
        : scene_entity(GxSceneManager::get().build("scene", 0.0))
    {
        const GxEndCaller end([this] { scene_entity->add_to_world(); });

        for (int metallic_i = 0; metallic_i < 10; ++metallic_i) {
            for (int roughness_i = 0; roughness_i < 10; ++roughness_i) {
                const auto metallic = 0.05f + static_cast<float>(metallic_i) * 0.1f;
                const auto roughness = 0.05f + static_cast<float>(roughness_i) * 0.1f;
                const auto postfix = "-metallic:" + std::to_string(metallic) + "-roughness:" + std::to_string(roughness);
                GxMaterialManager::get().get_pbr(
                    "icosphere-material-" + postfix, GxPbrEndCaller([this, metallic, roughness, p = postfix, e = end](GxPbrPtr&& material) mutable {
                        icosphere_material_is_ready(std::move(material), std::move(p), metallic, roughness, e);
                    }));
            }
        }

        GxMaterialManager::get().get_pbr("ground", GxPbrEndCaller([this, e = end](GxPbrPtr&& material) { plate_material_is_ready(std::move(material), e); }));

        GxCameraManager::get().build("camera", scene_entity.get(), GxEntityEndCaller([this, end](auto&& entity) {
            auto trn = entity->template get_component_shared_ptr<GxTransform>();
            trn->local_translate({ -19.0, -19.0, 5.0 });
            trn->local_look_at({ -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
            (void)GxConstraintManager::get().create_jet_controller(entity->get_object_name() + "-controller", std::move(trn), scene_entity.get());
            (void)end;
        }));

        for (int light_index = 0; light_index < 4; ++light_index) {
            GxLightManager::get().build_shadow_caster_directional(
                "directional-light-" + std::to_string(light_index), scene_entity.get(), 1024, 50.0f, 1.0f, 35.0f,
                GxEntityEndCaller([e = end, light_index](GxEntityPtr&& light_entity) {
                    auto* const light = light_entity->get_component<GxShadowCaster>();
                    light->get_shadow_transform()->local_look_at(
                        { static_cast<double>(((light_index & 1) << 1) - 1) * 11.0, static_cast<double>(((light_index >> 1) << 1) - 1) * 11.0, 11.0 },
                        { 0.0, 0.0, 0.0 },
                        { 0.0, 1.0, 0.0 });
                    light->colour = { 2.0f, 2.0f, 2.0f };
                    (void)e;
            }));
        }

        GX_LOG_D("Initialised");
    }

    void icosphere_mesh_is_ready(GxMeshPtr&& mesh, const std::string& postfix, const GxEndCaller&, const float metallic, const float roughness)
    {
        auto model_entity = GxModelManager::get().build("icosphere" + postfix, scene_entity.get(), { std::move(mesh) }, true);
        model_entity->get_component<GxTransform>()->local_translate({ static_cast<double>(metallic) * 30.0 - 15.0, static_cast<double>(roughness) * 30.0 - 15.0, 0.0 });
    }

    void icosphere_material_is_ready(GxPbrPtr&& material, std::string&& postfix, const float metallic, const float roughness, const GxEndCaller& end)
    {
        material->set_albedo_factor({ 0.999f, 0.1f, 0.4f, 1.0f });
        material->set_normal_metallic_factor({ 1.0f, 1.0f, 1.0f, metallic });
        material->set_emission_roughness_factor({ 1.0f, 1.0f, 1.0f, roughness });

        GxMeshManager::get().build_icosphere(4, std::move(material), GxMeshEndCaller([this, e = end, metallic, roughness, p = std::move(postfix)](GxMeshPtr&& mesh) {
            icosphere_mesh_is_ready(std::move(mesh), p, e, metallic, roughness);
        }));
    }

    void plate_mesh_is_ready(GxMeshPtr&& mesh, const GxEndCaller&)
    {
        auto model_entity = GxModelManager::get().build("ground", scene_entity.get(), { std::move(mesh) }, true);
        auto& trn = *model_entity->get_component<GxTransform>();
        trn.local_inner_scale(30.0);
        trn.local_translate({ 0.0, 0.0, -5.0 });
    }

    void plate_material_is_ready(GxPbrPtr&& material, const GxEndCaller& end)
    {
        material->set_albedo_factor({ 0.1f, 0.999f, 0.3f, 1.0f });
        material->set_normal_metallic_factor({ 1.0f, 1.0f, 1.0f, 0.001 });
        material->set_emission_roughness_factor({ 1.0f, 1.0f, 1.0f, 0.99f });

        GxMeshManager::get().build_plate(std::move(material), GxMeshEndCaller([this, e = end](GxMeshPtr&& mesh) { plate_mesh_is_ready(std::move(mesh), e); }));
    }
};

GEAROENIX_START(GameApp);
