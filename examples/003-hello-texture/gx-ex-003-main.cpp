#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>

struct GameApp final : public gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        std::vector<gearoenix::render::PbrVertex> vertices(3);
        vertices[0].set_position(1.0f, -1.0f, 0.0f);
        vertices[0].set_uv(1.0f, -1.0f);
        vertices[1].set_position(0.0f, 1.0f, 0.0f);
        vertices[1].set_uv(0.0f, 1.0f);
        vertices[2].set_position(-1.0f, -1.0f, 0.0f);
        vertices[2].set_uv(-1.0f, -1.0f);

        std::vector<std::uint32_t> indices = { 0, 2, 1 };

        const auto scene_builder = render_engine.get_scene_manager()->build("scene");

        auto end_callback = gearoenix::core::sync::EndCallerIgnored([scene_builder] {});

        auto model_builder = render_engine.get_model_manager()->build(
            "triangle",
            render_engine.get_mesh_manager()->build(
                "triangle-mesh",
                std::move(vertices),
                std::move(indices),
                gearoenix::core::sync::EndCallerIgnored(end_callback)),
            gearoenix::core::sync::EndCallerIgnored(end_callback),
            true);
        gearoenix::render::material::Pbr material(render_engine);
        std::vector<std::vector<std::uint8_t>> pixels(3);
        for (int j = 64 * 64, k = 0; k < 3; j >>= 2, ++k)
            for (int i = 0; i < j; ++i) {
                pixels[k].push_back(k == 0 ? 255 : 0);
                pixels[k].push_back(k == 1 ? 255 : 0);
                pixels[k].push_back(k == 2 ? 255 : 0);
                pixels[k].push_back(255);
            }
        material.set_albedo(render_engine.get_texture_manager()->create_2d_from_pixels(
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
        material.set_albedo(render_engine.get_texture_manager()->create_2d_from_file(
            "gearoenix-logo",
            //gearoenix::platform::AbsolutePath("../../../../assets/gearoenix-logo.png"),
            gearoenix::platform::AssetPath(plt_app, "logo.png"),
            gearoenix::render::texture::TextureInfo(),
            end_callback));
        model_builder->set_material(material);
        scene_builder->add(std::move(model_builder));

        auto camera_builder = render_engine.get_camera_manager()->build("camera");
        camera_builder->get_transformation().set_location(0.0f, 0.0f, 5.0f);
        scene_builder->add(std::move(camera_builder));
    }

    void update() noexcept final
    {
        Application::update();
    }
};

GEAROENIX_START(GameApp)
