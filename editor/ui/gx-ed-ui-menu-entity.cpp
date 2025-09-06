#include "gx-ed-ui-menu-entity.hpp"
#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-menu-scene.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-name-input-text.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-selector.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-style-wrong-input-text.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-type-table.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <filesystem>

namespace {
[[nodiscard]] bool is_valid_skybox_path(const std::string& path)
{
    return std::filesystem::exists(path) && (path.ends_with(".hdr") || path.ends_with(".jpg") || path.ends_with(".png") || path.ends_with(".gx-cube-texture"));
}
}

void gearoenix::editor::ui::MenuEntity::show_create_camera_window()
{
    if (!is_create_camera_open) {
        return;
    }

    constexpr char popup_id[] = "Create New Camera";

    if (!ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_AnyPopup)) {
        ImGui::OpenPopup(popup_id, ImGuiPopupFlags_NoReopen);
    }

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal(popup_id, &is_create_camera_open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        return;
    }

    bool new_scene_valid_name = false;

    render::imgui::table_scope("##gearoenix::editor::ui::MenuEntity::show_create_camera_window", [&] {
        ImGui::Text("Camera Entity Name:");
        ImGui::TableNextColumn();
        new_scene_valid_name = render::imgui::entity_name_text_input(create_camera_entity_name, 200.0f).second;
    });

    const float window_width = ImGui::GetWindowSize().x;

    constexpr auto button_width_ratio = 0.35f;
    constexpr auto spacing_width_ratio = (1.0f - button_width_ratio * 2.0f) / 3.0f;
    constexpr auto second_button_start_ratio = button_width_ratio + spacing_width_ratio * 2.0f;

    ImGui::SetCursorPosX(spacing_width_ratio * window_width);
    const auto button_width = window_width * button_width_ratio;
    if (ImGui::Button("Cancel", ImVec2(button_width, 0.0f))) {
        is_create_camera_open = false;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(second_button_start_ratio * window_width);
    if (new_scene_valid_name && ImGui::Button(popup_id, ImVec2(button_width, 0.0f))) {
        is_create_camera_open = false;
        render::camera::Manager::get().build(
            std::move(create_camera_entity_name),
            MenuScene::get().get_current_scene(),
            core::job::EndCaller<core::ecs::EntityPtr>([](auto&& camera_entity) {
                camera_entity->add_to_world();
            }));
    }

    ImGui::EndPopup();
}

void gearoenix::editor::ui::MenuEntity::show_create_skybox_window()
{
    constexpr char key_file_browser[] = "gearoenix/editor/entity/create/skybox/file-browser";

    if (!is_create_skybox_open) {
        return;
    }

    if (!ImGui::Begin("Create Skybox", &is_create_skybox_open)) {
        ImGui::End();
        return;
    }

    const auto is_valid_path = is_valid_skybox_path(create_skybox_path);

    (void)render::imgui::entity_name_text_input(create_skybox_entity_name);
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
        if (const auto scene_entity = scene_selector->get_selection(); scene_entity && ImGui::Button("Create")) {
            const auto progress_bar_id = WindowOverlayProgressBarManager::get().add("Loading Skybox [" + create_skybox_path + "]...");
            core::job::send_job_to_pool([this, scene_entity, progress_bar_id] {
                render::skybox::Manager::get().build(
                    std::string(create_skybox_entity_name),
                    scene_entity,
                    platform::stream::Path::create_absolute(create_skybox_path),
                    core::job::EndCaller<core::ecs::EntityPtr>([progress_bar_id](auto&& skybox_entity) {
                        WindowOverlayProgressBarManager::get().remove(progress_bar_id);
                        skybox_entity->add_to_world();
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
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Render")) {
        if (ImGui::MenuItem("Camera")) {
            is_create_camera_open = true;
        }
        if (ImGui::MenuItem("Skybox", "Ctrl+N,Ctrl+S")) {
            is_create_skybox_open = true;
        }
        ImGui::EndMenu();
    }
}

gearoenix::editor::ui::MenuEntity::MenuEntity()
    : scene_selector(std::make_unique<render::imgui::EntitySelector>())
{
}

gearoenix::editor::ui::MenuEntity::~MenuEntity() = default;

void gearoenix::editor::ui::MenuEntity::update()
{
    if (!MenuScene::get().has_active_scene()) {
        return;
    }

    if (ImGui::BeginMenu("Entity")) {
        if (ImGui::BeginMenu("Create")) {
            show_create_menu();
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Delete", "Alt+O,Alt+Del", false, false)) { }
        if (ImGui::MenuItem("Settings", "Ctrl+Alt+O", false, false)) { }
        ImGui::EndMenu();
    }

    show_create_camera_window();
    show_create_skybox_window();
}

void gearoenix::editor::ui::MenuEntity::renew()
{
    GX_TODO;
}
