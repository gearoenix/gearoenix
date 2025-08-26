#include "gx-ed-ui-menu-entity.hpp"
#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-name-input-text.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-selector.hpp>
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

    render::imgui::entity_name_text_input(create_skybox_entity_name);
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
}
void gearoenix::editor::ui::MenuEntity::renew()
{
    GX_TODO;
}
