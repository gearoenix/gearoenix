#include "gx-editor-ui-menu-entity.hpp"
#include "gx-editor-ui-manager.hpp"
#include "gx-editor-ui-window-overlay-progress-bar.hpp"
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <filesystem>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-name-input-text.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-selector.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-builder.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-skybox.hpp>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace {
[[nodiscard]] bool is_valid_skybox_path(const std::string& path)
{
    return std::filesystem::exists(path) && (path.ends_with(".hdr") || path.ends_with(".jpg") || path.ends_with(".png") || path.ends_with(".gx-cube-texture"));
}
}

void gearoenix::editor::ui::MenuEntity::show_create_physics_constraint_jet_controller()
{
    if (!is_create_physics_constraint_jet_controller) {
        return;
    }

    if (!ImGui::Begin("Create Physics>Constraint>JetController", &is_create_physics_constraint_jet_controller)) {
        ImGui::End();
        return;
    }

    auto& e = *manager.get_platform_application().get_base().get_render_engine();
    auto& w = *e.get_world();
    render::imgui::entity_name_text_input(w, create_physics_constraint_jet_controller_entity_name);

    transformation_selector->show<physics::TransformationComponent>();

    if (transformation_selector->selected() && !create_physics_constraint_jet_controller_entity_name.empty() && nullptr == w.get_entity(create_physics_constraint_jet_controller_entity_name) && ImGui::Button("Create")) {
        (void)e.get_physics_engine()->get_constraint_manager()->create_jet_controller(
            create_physics_constraint_jet_controller_entity_name + "",
            std::dynamic_pointer_cast<physics::Transformation>(
                w.get_component<physics::TransformationComponent>(transformation_selector->get_selection())->get_component_self().lock()),
            transformation_selector->get_selection(),
            core::job::EndCaller([] { }));
    }

    ImGui::End();
}

void gearoenix::editor::ui::MenuEntity::show_create_skybox_window()
{
    constexpr auto key_file_browser = "gearoenix/editor/entity/create/skybox/file-browser";

    if (!is_create_skybox_open) {
        return;
    }

    if (!ImGui::Begin("Create Skybox", &is_create_skybox_open)) {
        ImGui::End();
        return;
    }

    const auto is_valid_path = is_valid_skybox_path(create_skybox_path);

    auto& w = *manager.get_platform_application().get_base().get_render_engine()->get_world();
    render::imgui::entity_name_text_input(w, create_skybox_entity_name);
    {
        const auto _ = render::imgui::set_wrong_input_text_style(create_skybox_path.empty() || is_valid_path);
        ImGui::InputTextWithHint("Skybox Image Path", "Path to .hdr,.png,.jpg or gx-cube-texture", &create_skybox_path);
        (void)_;
        ImGui::SameLine();
        if (ImGui::Button("Browse File")) {
            ImGuiFileDialog::Instance()->OpenDialog(key_file_browser, "Import Skybox Image", ".hdr,.png,.jpg,.gx-cube-texture");
        }
    }

    scene_selector->show<render::scene::Scene>();

    if (is_valid_path) {
        if (ImGui::Button("Create")) {
            const auto progress_bar_id = manager.get_window_overlay_progree_bar_manager()->add("Loading Skybox [" + create_skybox_path + "]...");
            const auto scene_id = scene_selector->get_selection();
            core::job::send_job_io1([this, scene_id, progress_bar_id, w = &w] {
                manager.get_platform_application().get_base().get_render_engine()->get_skybox_manager()->build(
                    create_skybox_entity_name,
                    platform::stream::Path::create_absolute(create_skybox_path),
                    core::job::EndCaller([] {}),
                    core::job::EndCallerShared<render::skybox::Builder>([this, scene_id, progress_bar_id, w](auto&& skybox_builder) {
                        manager.get_window_overlay_progree_bar_manager()->remove(progress_bar_id);
                        if (core::ecs::INVALID_ENTITY_ID == scene_id) {
                            return;
                        }
                        auto* const s = w->get_component<render::scene::Scene>(scene_id);
                        auto* const sky = skybox_builder->get_skybox();
                        s->add_skybox(skybox_builder->get_entity_id(), *sky);
                        sky->set_scene_id(scene_id);
                    }));
            });
        }
        if (!create_skybox_path.ends_with(".gx-cube-texture")) {
            ImGui::SameLine();
            if (ImGui::Button("Create & Bake to cube map")) {
            }
        }
        ImGui::SameLine();
    }
    if (ImGui::Button("Cancel")) {
        is_create_skybox_open = false;
    }

    ImGui::End();

    if (ImGuiFileDialog::Instance()->Display(key_file_browser)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            create_skybox_path = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void gearoenix::editor::ui::MenuEntity::show_create_menu()
{
    if (ImGui::BeginMenu("Physics")) {
        if (ImGui::BeginMenu("Constraints")) {
            if (ImGui::MenuItem("Jet Controller")) {
                is_create_physics_constraint_jet_controller = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Render")) {
        if (ImGui::MenuItem("Skybox", "Ctrl+N,Ctrl+S")) {
            is_create_skybox_open = true;
        }
        ImGui::EndMenu();
    }
}

gearoenix::editor::ui::MenuEntity::MenuEntity(Manager& manager)
    : manager(manager)
    , transformation_selector(new render::imgui::EntitySelector(*manager.get_platform_application().get_base().get_render_engine()))
    , scene_selector(new render::imgui::EntitySelector(*manager.get_platform_application().get_base().get_render_engine()))
{
}

gearoenix::editor::ui::MenuEntity::~MenuEntity() = default;

void gearoenix::editor::ui::MenuEntity::update()
{
    if (ImGui::BeginMenu("Entity")) {
        if (ImGui::BeginMenu("Create")) {
            show_create_menu();
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Delete", "Alt+O,Alt+Del", false, false)) { }
        if (ImGui::MenuItem("Settings", "Ctrl+Alt+O", false, false)) { }
        ImGui::EndMenu();
    }

    show_create_skybox_window();
    show_create_physics_constraint_jet_controller();
}
