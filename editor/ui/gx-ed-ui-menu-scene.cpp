#include "gx-ed-ui-menu-scene.hpp"
#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <imgui/imgui.h>

namespace {
constexpr auto key_gltf_file_chooser = "key_gltf_file_chooser";
constexpr auto filter_gltf_file = ".glb,.gltf";
}

gearoenix::editor::ui::MenuScene::MenuScene(Manager& manager)
    : platform_application(manager.get_platform_application())
    , manager(manager)
{
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
            const auto progress_bar_id = manager.get_window_overlay_progree_bar_manager()->add("Loading Scenes from GLTF File...");
            auto file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
            render::gltf::load(
                *manager.get_platform_application().get_base().get_render_engine(),
                platform::stream::Path::create_absolute(std::move(file_path_name)),
                core::job::EndCaller<std::vector<std::shared_ptr<render::scene::Builder>>>([this, progress_bar_id](auto&& /*scene_builders*/) {
                    manager.get_window_overlay_progree_bar_manager()->remove(progress_bar_id);
                }),
                core::job::EndCaller([] {}));
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (core::ecs::invalid_entity_id == active_scene) {
        core::ecs::World::get()->synchronised_system<render::scene::Scene>([this](const auto id, auto* const scene) {
            if (core::ecs::invalid_entity_id != active_scene) {
                return;
            }
            active_scene = id;
            scene->set_enabled(true);
        });
    }
}