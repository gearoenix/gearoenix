#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-manager.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-runtime.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

template <typename T>
using GxEndCallerShared = gearoenix::core::job::EndCallerShared<T>;

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
using GxMaterialManager = gearoenix::render::material::Manager;
using GxPbr = gearoenix::render::material::Pbr;
using GxMeshManager = gearoenix::render::mesh::Manager;
using GxMesh = gearoenix::render::mesh::Mesh;
using GxModelManager = gearoenix::render::model::Manager;
using GxReflectionManager = gearoenix::render::reflection::Manager;
using GxReflectionRuntime = gearoenix::render::reflection::Runtime;
using GxSceneManager = gearoenix::render::scene::Manager;
using GxSkyboxManager = gearoenix::render::skybox::Manager;
using GxMeshEndCaller = GxEndCallerShared<GxMesh>;
using GxPbrEndCaller = GxEndCallerShared<GxPbr>;
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
                GxMaterialManager::get().get_pbr("material-" + postfix, GxPbrEndCaller([this, metallic, roughness, p = postfix, e = end](GxPbrPtr&& material) mutable {
                    material_is_ready(std::move(material), metallic, roughness, std::move(p), std::move(e));
                }));
            }
        }

        GxSkyboxManager::get().build("skybox", scene_entity.get(), GxPath::create_asset("sky.gx-cube-texture"), GxEntityEndCaller([end](auto&&) { (void)end; }));

        GxCameraManager::get().build("camera", scene_entity.get(), GxEntityEndCaller([this, end](auto&& entity) {
            auto trn = entity->template get_component_shared_ptr<GxTransform>();
            trn->local_translate({ -19.0, -19.0, 5.0 });
            trn->local_look_at({ -11.0, -11.0, 0.0 }, { 0.0, 0.0, 1.0 });
            (void)GxConstraintManager::get().create_jet_controller(entity->get_object_name() + "-controller", std::move(trn), scene_entity.get());
            (void)end;
        }));

        GxReflectionManager::get().build_baked(
            "baked-reflection", scene_entity.get(),
            GxPath::create_asset("exported.gx-reflection"),
            GxEntityEndCaller([end](auto&&) { (void)end; }));

        GX_LOG_D("Initialised");
    }

    void material_is_ready(GxPbrPtr&& material, const float metallic, const float roughness, std::string&& postfix, GxEndCaller&& end)
    {
        material->get_normal_metallic_factor().w = metallic;
        material->get_emission_roughness_factor().w = roughness;

        GxMeshManager::get().build_icosphere(4, std::move(material), GxMeshEndCaller([this, metallic, roughness, p = std::move(postfix), e = std::move(end)](GxMeshPtr&& m) mutable {
            mesh_is_ready(std::move(m), metallic, roughness, std::move(p), std::move(e));
        }));
    }

    void mesh_is_ready(GxMeshPtr&& mesh, const float metallic, const float roughness, std::string&& postfix, GxEndCaller&&)
    {
        auto entity = GxModelManager::get().build("icosphere" + postfix, scene_entity.get(), { std::move(mesh) }, true);
        entity->get_component<GxTransform>()->local_translate({ static_cast<double>(metallic) * 30.0 - 15.0, static_cast<double>(roughness) * 30.0 - 15.0, 0.0 });
    }
};

GEAROENIX_START(GameApp)
