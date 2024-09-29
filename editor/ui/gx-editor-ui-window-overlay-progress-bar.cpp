#include "gx-editor-ui-window-overlay-progress-bar.hpp"
#include <imgui/imgui.h>
#include <utility>

gearoenix::editor::ui::WindowOverlayProgressBar::WindowOverlayProgressBar(
    std::string progress_bar_text, std::optional<std::function<float()>>&& progress_indicator)
    : progress_bar_text(std::move(progress_bar_text))
    , progress_indicator(std::move(progress_indicator))
{
}

void gearoenix::editor::ui::WindowOverlayProgressBar::update() const
{
    bool always_show = true;
    ImGui::OpenPopup("Progress Bar");
    const ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(
            "Progress Bar",
            &always_show,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        const auto progress_bar_length = static_cast<float>(progress_bar_text.size()) * 10.0f;
        if (progress_indicator.has_value()) {
            const auto progress = (*progress_indicator)();
            ImGui::ProgressBar(progress, ImVec2(progress_bar_length, 0.0f), progress_bar_text.c_str());
        } else {
            ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(progress_bar_length, 0.0f), progress_bar_text.c_str());
        }
        ImGui::EndPopup();
    }
}

int gearoenix::editor::ui::WindowOverlayProgressBarManager::add(
    const std::string& progress_bar_text,
    std::optional<std::function<float()>>&& progress_indicator)
{
    const auto id = ++id_generator;
    bars.emplace(id, std::make_shared<WindowOverlayProgressBar>(progress_bar_text, std::move(progress_indicator)));
    return id;
}

void gearoenix::editor::ui::WindowOverlayProgressBarManager::remove(const int id)
{
    bars.erase(id);
}

void gearoenix::editor::ui::WindowOverlayProgressBarManager::update() const
{
    if (bars.empty()) {
        return;
    }
    bars.rbegin()->second->update();
}
