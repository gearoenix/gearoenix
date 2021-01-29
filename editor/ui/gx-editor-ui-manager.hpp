#ifndef GEAROENIX_EDITOR_UI_MANAGER_HPP
#define GEAROENIX_EDITOR_UI_MANAGER_HPP

#include "gx-editor-ui-menu-bar.hpp"

namespace gearoenix::editor::ui {
    struct Manager final {
    private:
        MenuBar menu_bar;
    public:
        void update() noexcept;
    };
}

#endif