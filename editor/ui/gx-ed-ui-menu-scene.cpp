#include "gx-ed-ui-menu-scene.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/gltf/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

#include <ImGui/imgui.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

namespace {
constexpr auto key_gltf_file_chooser = "key_gltf_file_chooser";
constexpr auto filter_gltf_file = ".glb,.gltf";
}

gearoenix::editor::ui::MenuScene::MenuScene()
{
    renew();
}

void gearoenix::editor::ui::MenuScene::update()
{
    if (ImGui::BeginMenu("Scene")) {
        if (ImGui::MenuItem("New", "Alt+S,Alt+N", false)) { }
        if (ImGui::MenuItem("Import", "Alt+S,Alt+I", false)) {
            ImGuiFileDialog::Instance()->OpenDialog(key_gltf_file_chooser, "Import GLTF file", filter_gltf_file);
        }
        if (ImGui::MenuItem("Delete", "Alt+S,Alt+Del", false, false)) { }
        if (ImGui::MenuItem("Settings", "Alt+S,Alt+S", false, false)) { }
        ImGui::EndMenu();
    }

    if (ImGuiFileDialog::Instance()->Display(key_gltf_file_chooser)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const auto progress_bar_id = WindowOverlayProgressBarManager::get().add("Loading Scenes from GLTF File...");
            auto file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
            render::gltf::load(
                platform::stream::Path::create_absolute(std::move(file_path_name)),
                core::job::EndCaller<std::vector<core::ecs::EntityPtr>>([this, progress_bar_id](auto&& entities) {
                    WindowOverlayProgressBarManager::get().remove(progress_bar_id);
                    for (auto& e : entities) {
                        active_scenes.emplace(std::move(e));
                    }
                }));
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (active_scenes.empty()) {
        core::ecs::World::get().synchronised_system<render::scene::Scene>([this](const auto* const entity, auto* const) {
            active_scenes.emplace(entity->get_ptr());
        });
    }
}
void gearoenix::editor::ui::MenuScene::renew()
{
    active_scenes.clear();
    active_scenes.emplace(render::scene::Manager::get().build("gx-default-scene", 0.0));
}