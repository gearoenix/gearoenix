#pragma once
#include <gearoenix/core/gx-cr-singleton.hpp>

#include <boost/container/flat_map.hpp>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

namespace gearoenix::editor::ui {
struct WindowOverlayProgressBar;
struct WindowOverlayProgressBarHandle;

struct WindowOverlayProgressBarManager final : core::Singleton<WindowOverlayProgressBarManager> {
private:
    std::atomic<int> id_generator = 0;
    std::mutex bars_lock;
    boost::container::flat_map<int, std::shared_ptr<WindowOverlayProgressBar>> bars;

public:
    WindowOverlayProgressBarManager();
    ~WindowOverlayProgressBarManager() override;
    [[nodiscard]] std::shared_ptr<WindowOverlayProgressBarHandle> add(std::string&& progress_bar_text, std::optional<std::function<float()>>&& progress_indicator = std::nullopt);
    void remove(int id);
    void update();
};
}