#include "gx-ed-ui-menu-scene.hpp"
#include "../gx-editor-main.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/gltf/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-name-input-text.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-type-table.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

#include <ImGui/imgui.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <format>

namespace {
constexpr auto key_gltf_file_chooser = "key_gltf_file_chooser";
constexpr auto filter_gltf_file = ".glb,.gltf";
}

void gearoenix::editor::ui::MenuScene::show_new_popup()
{
    if (!is_new_popup_open) {
        return;
    }

    constexpr char popup_id[] = "Create New Scene";

    if (!ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_AnyPopup)) {
        ImGui::OpenPopup(popup_id, ImGuiPopupFlags_NoReopen);
    }

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal(popup_id, &is_new_popup_open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        return;
    }

    bool new_scene_valid_name = false;

    render::imgui::table_scope("##gearoenix::core::ecs::Component", [&] {
        ImGui::Text("Scene Name:");
        ImGui::TableNextColumn();
        new_scene_valid_name = render::imgui::entity_name_text_input(new_scene_name, 200.0f).second;
        ImGui::TableNextColumn();

        ImGui::Text("Scene Priority:");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputFloat("##" GX_STRINGIFY(scene_priority), &new_scene_priority);
    });

    const float window_width = ImGui::GetWindowSize().x;

    constexpr auto button_width_ratio = 0.35f;
    constexpr auto spacing_width_ratio = (1.0f - button_width_ratio * 2.0f) / 3.0f;
    constexpr auto second_button_start_ratio = button_width_ratio + spacing_width_ratio * 2.0f;

    ImGui::SetCursorPosX(spacing_width_ratio * window_width);
    const auto button_width = window_width * button_width_ratio;
    if (ImGui::Button("Cancel", ImVec2(button_width, 0.0f))) {
        is_new_popup_open = false;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(second_button_start_ratio * window_width);
    if (new_scene_valid_name && ImGui::Button("Create New Scene", ImVec2(button_width, 0.0f))) {
        is_new_popup_open = false;
        auto entity = render::scene::Manager::get().build(std::move(new_scene_name), new_scene_priority);
        entity->add_to_world();
        set_current_scene(entity.get());
        active_scenes.emplace(std::move(entity));
    }

    ImGui::EndPopup();
}

void gearoenix::editor::ui::MenuScene::set_current_scene(core::ecs::Entity* const scene)
{
    if (current_scene == scene) {
        return;
    }
    current_scene = scene;
    Singleton<EditorApplication>::get().set_current_scene(scene);
}

gearoenix::editor::ui::MenuScene::MenuScene()
    : Singleton(this)
{
    renew();
}

gearoenix::editor::ui::MenuScene::~MenuScene() = default;

void gearoenix::editor::ui::MenuScene::update()
{
    if (ImGui::BeginMenu("Scene")) {
        if (ImGui::MenuItem("New", "Alt+S,Alt+N", false)) {
            is_new_popup_open = true;
        }
        if (ImGui::MenuItem("Import", "Alt+S,Alt+I", false)) {
            ImGuiFileDialog::Instance()->OpenDialog(key_gltf_file_chooser, "Import GLTF file", filter_gltf_file);
            is_gltf_popup_open = true;
        }
        if (ImGui::BeginMenu("Scenes")) {
            ImGui::Text("Active scenes: %zu", static_cast<std::size_t>(active_scenes.size()));
            ImGui::Separator();

            if (ImGui::MenuItem("Refresh list")) {
                auto scenes = decltype(active_scenes) {};
                core::ecs::World::get().synchronised_system<render::scene::Scene>([&](const auto* const entity, auto* const) {
                    scenes.emplace(entity->get_ptr());
                });
                if (!current_scene || !scenes.contains(current_scene->get_ptr())) {
                    if (scenes.empty()) {
                        set_current_scene(nullptr);
                    } else {
                        set_current_scene(scenes.begin()->get());
                    }
                }
                active_scenes = std::move(scenes);
            }

            ImGui::Separator();

            boost::container::flat_set<core::ecs::EntityPtr> to_remove;
            std::size_t idx = 0;
            for (const auto& e : active_scenes) {
                ++idx;
                const std::string label = std::format("Scene [{}]: {}", idx, e->get_object_name());

                ImGui::PushID(static_cast<int>(e->get_object_id()));
                if (ImGui::BeginMenu(label.c_str())) {
                    if (ImGui::MenuItem("Delete")) {
                        to_remove.emplace(e);
                    }
                    if (ImGui::MenuItem("Settings")) { }
                    ImGui::EndMenu();
                }
                ImGui::PopID();
            }

            for (auto& e : to_remove) {
                if (e.get() == current_scene) {
                    active_scenes.erase(e);
                    if (active_scenes.empty()) {
                        set_current_scene(nullptr);
                    } else {
                        set_current_scene(active_scenes.begin()->get());
                    }
                } else {
                    active_scenes.erase(e);
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if (is_gltf_popup_open && ImGuiFileDialog::Instance()->Display(key_gltf_file_chooser)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const auto progress_bar_id = WindowOverlayProgressBarManager::get().add("Loading Scenes from GLTF File...");
            auto file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
            render::gltf::load(
                platform::stream::Path::create_absolute(std::move(file_path_name)),
                core::job::EndCaller<std::vector<core::ecs::EntityPtr>>([this, progress_bar_id](auto&& entities) {
                    WindowOverlayProgressBarManager::get().remove(progress_bar_id);
                    for (auto& e : entities) {
                        set_current_scene(e.get());
                        active_scenes.emplace(std::move(e));
                    }
                }));
        }
        is_gltf_popup_open = ImGuiFileDialog::Instance()->IsOpened();
        ImGuiFileDialog::Instance()->Close();
    }

    show_new_popup();
}
void gearoenix::editor::ui::MenuScene::renew()
{
    active_scenes.clear();
    set_current_scene(nullptr);
}

bool gearoenix::editor::ui::MenuScene::has_active_scene() const
{
    return !active_scenes.empty();
}