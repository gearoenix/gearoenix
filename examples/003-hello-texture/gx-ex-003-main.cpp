#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>

typedef gearoenix::core::job::EndCaller<> GxEndCaller;

typedef gearoenix::render::scene::Scene GxScene;
typedef gearoenix::render::scene::Builder GxSceneBuilder;
typedef std::shared_ptr<GxSceneBuilder> GxSceneBuilderPtr;

typedef gearoenix::render::material::Pbr GxPbr;
typedef gearoenix::core::job::EndCallerShared<GxPbr> GxPbrEndCaller;
typedef std::shared_ptr<GxPbr> GxPbrPtr;

typedef gearoenix::render::texture::TextureInfo GxTextureInfo;
typedef gearoenix::render::texture::TextureFormat GxTextureFormat;
typedef gearoenix::render::texture::Type GxTextureType;
typedef gearoenix::render::texture::Texture2D GxTexture2D;
typedef gearoenix::core::job::EndCallerShared<GxTexture2D> GxTexture2DEndCaller;
typedef std::shared_ptr<GxTexture2D> GxTexture2DPtr;

typedef gearoenix::render::mesh::Mesh GxMesh;
typedef gearoenix::core::job::EndCallerShared<GxMesh> GxMeshEndCaller;
typedef std::shared_ptr<GxMesh> GxMeshPtr;

typedef gearoenix::render::camera::Builder GxCameraBuilder;
typedef gearoenix::core::job::EndCallerShared<GxCameraBuilder> GxCameraBuilderEndCaller;
typedef std::shared_ptr<GxCameraBuilder> GxCameraBuilderPtr;
typedef gearoenix::render::camera::JetController GxJetController;

typedef gearoenix::render::light::Builder GxLightBuilder;
typedef gearoenix::core::job::EndCallerShared<GxLightBuilder> GxLightBuilderEndCaller;
typedef std::shared_ptr<GxLightBuilder> GxLightBuilderPtr;

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<GxJetController> camera_controller;
    gearoenix::core::ecs::entity_id_t scene_id;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        render_engine.get_material_manager()->get_pbr(
            "material",
            GxPbrEndCaller([this](GxPbrPtr&& m) { material_is_ready(std::move(m)); }));
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
        render_engine.get_texture_manager()->create_2d_from_pixels(
            "coloured-mipmaps",
            std::move(pixels),
            GxTextureInfo()
                .set_format(GxTextureFormat::RgbaUint8)
                .set_width(64)
                .set_height(64)
                .set_type(GxTextureType::Texture2D),
            GxTexture2DEndCaller([this, m = std::move(material)](GxTexture2DPtr&& t) mutable { texture_is_ready(std::move(m), std::move(t)); }));

        // Or you can load an image, before that make sure you have the image in the assets folder
        //         render_engine.get_texture_manager()->create_2d_from_file(
        //            "gearoenix-logo",
        //            // gearoenix::platform::AbsolutePath("../../../../assets/gearoenix-logo.png"),
        //            gearoenix::platform::stream::Path::create_asset("logo.png"),
        //            GxTextureInfo(),
        //            GxTexture2DEndCaller([this, m = std::move(material)](GxTexture2DPtr&& t) mutable {
        //                    texture_is_ready(std::move(m), std::move(t));}));
    }

    void texture_is_ready(GxPbrPtr&& material, GxTexture2DPtr&& texture)
    {
        material->set_albedo(std::move(texture));

        render_engine.get_mesh_manager()->build_plate(
            std::move(material),
            GxMeshEndCaller([this](GxMeshPtr&& mesh) mutable {
                mesh_is_ready(std::move(mesh));
            }));
    }

    void mesh_is_ready(GxMeshPtr&& mesh)
    {
        auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0,
            GxEndCaller([this] {
                // Scene is hidden when it has been loaded, so we need to show it by following line:
                render_engine.get_world()->get_component<GxScene>(scene_id)->set_enabled(true);
            }));
        scene_id = scene_builder->get_id();

        auto model_builder = render_engine.get_model_manager()->build(
            "triangle", nullptr,
            { std::move(mesh) },
            GxEndCaller([] {
                // Here, it is not important for us to know when the model entity is actually in the world and
                // functioning, if it was, we had to add some code for handling this callback properly.
            }),
            true);
        scene_builder->add(std::move(model_builder));

        render_engine.get_camera_manager()->build(
            "camera", nullptr,
            GxCameraBuilderEndCaller([this, sb = std::move(scene_builder)](GxCameraBuilderPtr&& cb) mutable {
                camera_is_ready(std::move(cb), std::move(sb));
            }),
            GxEndCaller([] {
                // Here, it is not important for us to know when the camera entity is actually in the world and
                // functioning, if it was, we had to add some code for handling this callback properly.
            }));
    }

    void camera_is_ready(GxCameraBuilderPtr&& camera_builder, GxSceneBuilderPtr&& scene_builder)
    {
        camera_builder->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });
        camera_controller = std::make_unique<GxJetController>(render_engine, camera_builder->get_id());
        camera_controller->set_movement_speed(5.0);
        scene_builder->add(std::move(camera_builder));

        render_engine.get_light_manager()->build_shadow_caster_directional(
            "directional-light",
            nullptr,
            1024,
            10.0f,
            1.0f,
            10.0f,
            GxLightBuilderEndCaller([sb = std::move(scene_builder)](GxLightBuilderPtr&& lb) mutable {
                light_is_ready(std::move(sb), std::move(lb));
            }),
            GxEndCaller([] {
                // Here, it is not important for us to know when the light entity is actually in the world and
                // functioning, if it was, we had to add some code for handling this callback properly.
            }));
    }

    static void light_is_ready(GxSceneBuilderPtr&& scene_builder, GxLightBuilderPtr&& light_builder)
    {
        light_builder->get_shadow_caster_directional()->get_shadow_transform()->local_look_at(
            { 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
        light_builder->get_light().colour = { 8.0f, 8.0f, 8.0f };
        scene_builder->add(std::move(light_builder));
        (void)std::move(scene_builder);
        GX_LOG_D("Initialised");
    }

    void update() override
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)
