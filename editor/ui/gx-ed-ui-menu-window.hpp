#pragma once

namespace gearoenix::editor::ui {
struct MenuWindow final {
private:
    bool show_engine_inspector = false;

public:
    MenuWindow();
    void update();
    void renew();
};
}