#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>

#include <ImGui/imgui.h>

#include <string>
#include <utility>

namespace gearoenix::editor::ui {
struct WindowOverlayProgressBar final {
    GX_GET_REFC_PRV(std::string, progress_bar_text);
    GX_GET_REFC_PRV(std::optional<std::function<float()>>, progress_indicator);

public:
    WindowOverlayProgressBar(std::string&& progress_bar_text, std::optional<std::function<float()>>&& progress_indicator)
        : progress_bar_text(std::move(progress_bar_text))
        , progress_indicator(std::move(progress_indicator))
    {
    }

    void update() const
    {
        constexpr char progress_bar_id[] = "gearoenix::editor::ui::WindowOverlayProgressBar::update";
        if (!ImGui::IsPopupOpen(progress_bar_id, ImGuiPopupFlags_AnyPopup)) {
            ImGui::OpenPopup(progress_bar_id, ImGuiPopupFlags_NoReopen);
        }
        const ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal(
                progress_bar_id, nullptr,
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
};

struct WindowOverlayProgressBarHandle final {
    const int id;

    explicit WindowOverlayProgressBarHandle(const int id) noexcept
        : id(id)
    {
    }

    ~WindowOverlayProgressBarHandle() noexcept
    {
        WindowOverlayProgressBarManager::get().remove(id);
    }
};
}

gearoenix::editor::ui::WindowOverlayProgressBarManager::WindowOverlayProgressBarManager()
    : Singleton(this)
{
}

gearoenix::editor::ui::WindowOverlayProgressBarManager::~WindowOverlayProgressBarManager() = default;

std::shared_ptr<gearoenix::editor::ui::WindowOverlayProgressBarHandle> gearoenix::editor::ui::WindowOverlayProgressBarManager::add(
    std::string&& progress_bar_text,
    std::optional<std::function<float()>>&& progress_indicator)
{
    const auto id = ++id_generator;
    auto handle = std::make_shared<WindowOverlayProgressBarHandle>(id);
    const std::lock_guard _(bars_lock);
    bars.emplace(id, std::make_shared<WindowOverlayProgressBar>(std::move(progress_bar_text), std::move(progress_indicator)));
    return handle;
}

void gearoenix::editor::ui::WindowOverlayProgressBarManager::remove(const int id)
{
    const std::lock_guard _(bars_lock);
    bars.erase(id);
}

void gearoenix::editor::ui::WindowOverlayProgressBarManager::update()
{
    if (bars.empty()) {
        return;
    }
    const std::lock_guard _(bars_lock);
    bars.rbegin()->second->update();
}
