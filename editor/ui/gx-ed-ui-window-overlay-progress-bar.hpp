#pragma once
#include <atomic>
#include <boost/container/flat_map.hpp>
#include <functional>
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>
#include <optional>
#include <string>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::control {
struct Manager;
}

namespace gearoenix::editor::ui {
struct MenuBar;
struct WindowScene;
struct WindowOverlayProgressBar final {
    GX_GET_REFC_PRV(std::string, progress_bar_text);
    GX_GET_REFC_PRV(std::optional<std::function<float()>>, progress_indicator);

public:
    WindowOverlayProgressBar(
        std::string progress_bar_text,
        std::optional<std::function<float()>>&& progress_indicator);
    void update() const;
};

struct WindowOverlayProgressBarManager final {
private:
    std::atomic<int> id_generator = 0;
    boost::container::flat_map<int, std::shared_ptr<WindowOverlayProgressBar>> bars;

public:
    [[nodiscard]] int add(
        const std::string& progress_bar_text,
        std::optional<std::function<float()>>&& progress_indicator = std::nullopt);
    void remove(int id);
    void update() const;
};
}