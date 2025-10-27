#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>

typedef gearoenix::core::ecs::EntityPtr GxEntityPtr;
typedef gearoenix::core::job::EndCaller<> GxEndCaller;
typedef gearoenix::physics::Transformation GxTransform;
typedef gearoenix::physics::constraint::Manager GxConstraintManager;
typedef gearoenix::platform::stream::Path GxPath;
typedef gearoenix::render::camera::Manager GxCamManager;
typedef gearoenix::render::light::Manager GxLightManager;
typedef gearoenix::render::light::ShadowCasterDirectional GxShadowCaster;
typedef gearoenix::render::material::Manager GxMatManager;
typedef gearoenix::render::material::Pbr GxPbr;
typedef gearoenix::render::mesh::Manager GxMeshManager;
typedef gearoenix::render::mesh::Mesh GxMesh;
typedef gearoenix::render::model::Manager GxMdlManager;
typedef gearoenix::render::scene::Manager GxSceneManager;
typedef gearoenix::render::scene::Scene GxScene;
typedef gearoenix::render::texture::Manager GxTexManager;
typedef gearoenix::render::texture::Texture2D GxTexture2D;
typedef gearoenix::render::texture::TextureInfo GxTextureInfo;
typedef gearoenix::render::texture::TextureFormat GxTextureFormat;
typedef gearoenix::render::texture::Type GxTextureType;

typedef gearoenix::core::job::EndCaller<GxEntityPtr> GxEntityEndCaller;
typedef gearoenix::core::job::EndCallerShared<GxMesh> GxMeshEndCaller;
typedef gearoenix::core::job::EndCallerShared<GxPbr> GxPbrEndCaller;
typedef gearoenix::core::job::EndCallerShared<GxTexture2D> GxTexture2DEndCaller;
typedef std::shared_ptr<GxMesh> GxMeshPtr;
typedef std::shared_ptr<GxPbr> GxPbrPtr;
typedef std::shared_ptr<GxTexture2D> GxTexture2DPtr;

struct GameApp final : gearoenix::core::Application {
private:
    GxEntityPtr scene_entity;

public:
    GameApp()
        : scene_entity(GxSceneManager::get().build("scene", 0.0))
    {
        GxMatManager::get().get_pbr("material", GxPbrEndCaller([this](GxPbrPtr&& m) -> void {
            material_is_ready(std::move(m));
        }));
    }

    void material_is_ready(GxPbrPtr&& material)
    {
        std::vector<std::vector<std::uint8_t>> pixels(3);
        for (int j = 64 * 64, k = 0; k < 3; j >>= 2, ++k) {
            for (int i = 0; i < j; ++i) {
                pixels[k].push_back(k == 0 ? 255 : 0);
                pixels[k].push_back(k == 1 ? 255 : 0);
                pixels[k].push_back(k == 2 ? 255 : 0);
                pixels[k].push_back(255);
            }
        }

        constexpr bool use_texture_file = false;
        constexpr bool use_abs_path = false;

        if constexpr (use_texture_file) {
            // Or you can load an image before that make sure you have the image in the assets' folder.
            GxTexManager::get().create_2d_from_file(
                use_abs_path ? GxPath::create_absolute("../../../../assets/gearoenix-logo.png") : GxPath::create_asset("logo.png"),
                GxTextureInfo(),
                GxTexture2DEndCaller([this, m = std::move(material)](GxTexture2DPtr&& t) mutable {
                    texture_is_ready(std::move(m), std::move(t));
                }));
        } else {
            GxTexManager::get().create_2d_from_pixels(
                "coloured-mipmaps",
                std::move(pixels),
                GxTextureInfo()
                    .set_format(GxTextureFormat::RgbaUint8)
                    .set_width(64)
                    .set_height(64)
                    .set_type(GxTextureType::Texture2D),
                GxTexture2DEndCaller([this, m = std::move(material)](GxTexture2DPtr&& t) mutable {
                    texture_is_ready(std::move(m), std::move(t));
                }));
        }
    }

    void texture_is_ready(GxPbrPtr&& material, GxTexture2DPtr&& texture)
    {
        material->set_albedo(std::move(texture));

        GxMeshManager::get().build_plate(
            std::move(material),
            GxMeshEndCaller([this](GxMeshPtr&& mesh) -> void {
                mesh_is_ready(std::move(mesh));
            }));
    }

    void mesh_is_ready(GxMeshPtr&& mesh)
    {
        auto model_builder = GxMdlManager::get().build(
            "triangle", scene_entity.get(), { std::move(mesh) }, false);

        GxCamManager::get().build(
            "camera", scene_entity.get(), GxEntityEndCaller([this](GxEntityPtr&& e) -> void {
                camera_is_ready(std::move(e));
            }));
    }

    void camera_is_ready(GxEntityPtr&& camera_entity)
    {
        auto trn = camera_entity->get_component_shared_ptr<GxTransform>();
        trn->set_local_position({ 0.0f, 0.0f, 5.0f });

        (void)GxConstraintManager::get().create_jet_controller(
            camera_entity->get_object_name() + "-controller", std::move(trn), scene_entity.get());

        GxLightManager::get().build_shadow_caster_directional(
            "directional-light", scene_entity.get(), 1024,
            10.0f, 1.0f, 10.0f,
            GxEntityEndCaller([this](GxEntityPtr&& e) -> void {
                light_is_ready(std::move(e));
            }));
    }

    void light_is_ready(GxEntityPtr&& light_entity)
    {
        auto* const light = light_entity->get_component<GxShadowCaster>();
        light->get_shadow_transform()->local_look_at(
            { 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
        light->colour = { 8.0f, 8.0f, 8.0f };

        scene_entity->add_to_world();
        GX_LOG_D("Initialised");
    }
};

GEAROENIX_START(GameApp);
