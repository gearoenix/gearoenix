#ifndef GEAROENIX_EDITOR_UI_WINDOW_OVERLAY_PROGRESS_BAR_HPP
#define GEAROENIX_EDITOR_UI_WINDOW_OVERLAY_PROGRESS_BAR_HPP
#include <atomic>
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <string>
#include <optional>
#include <functional>
#include <boost/container/flat_map.hpp>
#include <memory>

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
            std::string  progress_bar_text,
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

#endif