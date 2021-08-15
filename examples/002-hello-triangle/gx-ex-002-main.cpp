#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-builder.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <imgui.h>

struct GameApp final : public gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application* plt_app) noexcept
        : Application(plt_app)
    {

        std::vector<gearoenix::render::PbrVertex> vertices(3);
        vertices[0].set_position(1.0f, 0.0f, 0.0f);
        vertices[1].set_position(0.0f, 1.0f, 0.0f);
        vertices[2].set_position(-1.0f, 0.0f, 0.0f);

        std::vector<std::uint32_t> indices = { 0, 1, 2 };

        auto mesh_builder = render_engine->get_mesh_manager()->build("triangle", vertices, indices);
        mesh_builder->set_material(gearoenix::render::material::Pbr());

        auto camera_builder = render_engine->get_camera_manager()->build();
        camera_builder->get_transformation()->set_position(0.0f, 0.0f, 5.0f);
        camera_builder->set_projection();

        const auto scene_builder = render_engine->get_scene_manager()->build();
        scene_builder->add(std::move(mesh_builder));
        scene_builder->add(std::move(camera_builder));
    }

    void update() noexcept final
    {
        static bool show_window = true;
        Application::update();

        ImGui::ShowDemoWindow(&show_window);

        ImGui::Begin("Hello Window!", &show_window);
        ImGui::Text("Hello Label!");
        if (ImGui::Button("Close"))
            show_window = false;
        ImGui::End();
    }
};

GEAROENIX_START(GameApp)