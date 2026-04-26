#include "gx-ed-ui-menu-scene.hpp"
#include "../gx-editor-main.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/gx-plt-file-chooser.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-stream.hpp>
#include <gearoenix/render/gltf/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-entity-name-input-text.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-type-table.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

#include <ImGui/imgui.h>

#include <format>
#include <iterator>
#include <string>

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

    render::imgui::table_scope("##gearoenix::editor::ui::MenuScene::show_new_popup", [&] {
        ImGui::Text("Scene Name:");
        ImGui::TableNextColumn();
        new_scene_valid_name = render::imgui::entity_name_text_input(new_scene_name, 200.0f).second;
        ImGui::TableNextColumn();

        ImGui::Text("Scene Priority:");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputFloat("##" GX_STRINGIFY(scene_priority), &new_scene_priority);
    });

    // Button widths are driven by the widest label text, not by a fraction of the
    // window — otherwise long labels like "Create New Scene" truncate inside a small
    // auto-resized window. Each button gets the same width (so they read as a pair),
    // and the row is centred within the available content region.
    const auto& style = ImGui::GetStyle();
    const float label_w = std::max(
        ImGui::CalcTextSize("Cancel").x, ImGui::CalcTextSize("Create New Scene").x);
    const float button_width = label_w + style.FramePadding.x * 2.0f + ImGui::GetFontSize();
    const float buttons_row_w = button_width * 2.0f + style.ItemSpacing.x;
    const float row_indent = std::max(0.0f, (ImGui::GetContentRegionAvail().x - buttons_row_w) * 0.5f);

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + row_indent);
    if (ImGui::Button("Cancel", ImVec2(button_width, 0.0f))) {
        is_new_popup_open = false;
    }
    ImGui::SameLine();
    if (new_scene_valid_name && ImGui::Button("Create New Scene", ImVec2(button_width, 0.0f))) {
        is_new_popup_open = false;
        render::scene::Manager::get().build(
            std::move(new_scene_name), new_scene_priority,
            core::job::EndCaller<core::ecs::EntityPtr>([this](core::ecs::EntityPtr&& entity) {
                entity->add_to_world();
                set_current_scene(entity.get());
                active_scenes.emplace(std::move(entity));
            }));
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
            platform::file_chooser_open(
                [this](platform::stream::Path&&, std::shared_ptr<platform::stream::Stream>&& stream) {
                    auto progress_bar = WindowOverlayProgressBarManager::get().add("Loading Scenes from GLTF File...");
                    render::gltf::load(std::move(stream), core::job::EndCaller<std::vector<core::ecs::EntityPtr>>([this, progress_bar = std::move(progress_bar)](auto&& entities) {
                        for (auto& e : entities) {
                            set_current_scene(e.get());
                            e->add_to_world();
                            active_scenes.emplace(std::move(e));
                        }
                        (void)progress_bar;
                    }));
                },
                [] { }, "Import GLTF file", ".glb" /*because of webassembly support we cant have gltf*/);
        }
        if (ImGui::BeginMenu("Scenes")) {
            ImGui::Text("Active scenes: %zu", static_cast<std::size_t>(active_scenes.size()));
            ImGui::Separator();

            if (ImGui::MenuItem("Refresh list")) {
                auto scenes = decltype(active_scenes) { };
                core::ecs::World::get().synchronised_system<render::scene::Scene>([&](const auto* const entity, auto* const) { scenes.emplace(entity->get_ptr()); });
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
            // Reused scratch `std::string` whose capacity sticks across frames — written
            // into via `std::format_to(back_inserter)` so there's no per-frame heap
            // allocation per scene. Safe because the editor UI is single-threaded and
            // ImGui consumes the label synchronously inside `BeginMenu`.
            static std::string label;
            std::size_t idx = 0;
            for (const auto& e : active_scenes) {
                ++idx;
                label.clear();
                std::format_to(std::back_inserter(label), "Scene [{}]: {}", idx, e->get_object_name());

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

    show_new_popup();
}
void gearoenix::editor::ui::MenuScene::renew()
{
    active_scenes.clear();
    set_current_scene(nullptr);
}

void gearoenix::editor::ui::MenuScene::add_active_scene(core::ecs::EntityPtr&& e)
{
    if (e->get_parent()) {
        return;
    }
    if (!e->get_component<render::scene::Scene>()) {
        return;
    }
    active_scenes.emplace(std::move(e));
}

bool gearoenix::editor::ui::MenuScene::has_active_scene() const
{
    return !active_scenes.empty();
}