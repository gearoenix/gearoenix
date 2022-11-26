#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
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
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;
    gearoenix::core::ecs::entity_id_t scene_id;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        std::vector<gearoenix::render::PbrVertex> vertices(3);
        vertices[0].position = { 1.0f, -1.0f, 0.0f };
        vertices[0].uv = { 1.0f, -1.0f };
        vertices[1].position = { 0.0f, 1.0f, 0.0f };
        vertices[1].uv = { 0.0f, 1.0f };
        vertices[2].position = { -1.0f, -1.0f, 0.0f };
        vertices[2].uv = { -1.0f, -1.0f };

        std::vector<std::uint32_t> indices = { 0, 1, 2 };

        auto end_callback = gearoenix::core::sync::EndCaller([this] {
            // Scene is hidden when it has been loaded, so we need to show it by following line:
            render_engine.get_world()->get_component<gearoenix::render::scene::Scene>(scene_id)->enabled = true;
        });

        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, gearoenix::core::sync::EndCaller(end_callback));
        scene_id = scene_builder->get_id();

        auto material = render_engine.get_material_manager()->get_pbr("material", end_callback);

        std::vector<std::vector<std::uint8_t>> pixels(3);
        for (int j = 64 * 64, k = 0; k < 3; j >>= 2, ++k)
            for (int i = 0; i < j; ++i) {
                pixels[k].push_back(k == 0 ? 255 : 0);
                pixels[k].push_back(k == 1 ? 255 : 0);
                pixels[k].push_back(k == 2 ? 255 : 0);
                pixels[k].push_back(255);
            }
        material->set_albedo(render_engine.get_texture_manager()->create_2d_from_pixels(
            "coloured-mipmaps",
            pixels,
            gearoenix::render::texture::TextureInfo {
                .format = gearoenix::render::texture::TextureFormat::RgbaUint8,
                .width = 64,
                .height = 64,
                .type = gearoenix::render::texture::Type::Texture2D,
            },
            end_callback));
        // Or you can load a image, before that make sure you have the image in the assets folder
        // material.set_albedo(render_engine.get_texture_manager()->create_2d_from_file(
        //    "gearoenix-logo",
        //    // gearoenix::platform::AbsolutePath("../../../../assets/gearoenix-logo.png"),
        //    gearoenix::platform::AssetPath(plt_app, "logo.png"),
        //    gearoenix::render::texture::TextureInfo(),
        //    end_callback));
        auto model_builder = render_engine.get_model_manager()->build(
            "triangle",
            render_engine.get_mesh_manager()->build(
                "triangle-mesh",
                std::move(vertices),
                std::move(indices),
                gearoenix::core::sync::EndCaller(end_callback)),
            std::move(material),
            gearoenix::core::sync::EndCaller(end_callback),
            true);
        scene_builder->add(std::move(model_builder));

        auto camera_builder = render_engine.get_camera_manager()->build("camera", gearoenix::core::sync::EndCaller(end_callback));
        camera_builder->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });
        camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
            render_engine,
            camera_builder->get_entity_builder()->get_builder().get_id());
        scene_builder->add(std::move(camera_builder));

        auto light_builder_0 = render_engine.get_light_manager()->build_shadow_caster_directional(
            "directional-light-0",
            1024,
            10.0f,
            1.0f,
            35.0f,
            end_callback);
        light_builder_0->get_transformation().local_look_at({ 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
        light_builder_0->get_light().colour = { 2.0f, 2.0f, 2.0f };
        scene_builder->add(std::move(light_builder_0));

        GX_LOG_D("Initialised");
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)
