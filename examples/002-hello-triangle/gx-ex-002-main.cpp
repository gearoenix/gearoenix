#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>

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

        std::vector<std::uint32_t> indices = { 0, 1, 2 };

        const auto scene_builder = render_engine.get_scene_manager()->build("scene");

        auto end_callback = gearoenix::core::sync::EndCallerIgnored([scene_builder] {});

        auto model_builder = render_engine.get_model_manager()->build(
            "triangle-model",
            render_engine.get_mesh_manager()->build("triangle-mesh", std::move(vertices), std::move(indices)),
            gearoenix::core::sync::EndCallerIgnored(end_callback),
            true);
        model_builder->set_material(gearoenix::render::material::Pbr(render_engine));

        auto camera_builder = render_engine.get_camera_manager()->build("camera");
        camera_builder->get_transformation().set_local_location({0.0f, 0.0f, 5.0f});

        scene_builder->add(std::move(model_builder));
        scene_builder->add(std::move(camera_builder));
    }

    void update() noexcept final
    {
        Application::update();
    }
};

GEAROENIX_START(GameApp)