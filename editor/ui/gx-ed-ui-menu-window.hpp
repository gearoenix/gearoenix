#pragma once

namespace gearoenix::editor::ui {
struct Manager;
struct MenuWindow final {
private:
    Manager& manager;
    bool show_engine_inspector = false;

public:
    explicit MenuWindow(Manager& manager);
    void update();
};
}