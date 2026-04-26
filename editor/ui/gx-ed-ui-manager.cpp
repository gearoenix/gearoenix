#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-components.hpp"
#include "gx-ed-ui-entities.hpp"
#include "gx-ed-ui-icons.hpp"
#include "gx-ed-ui-menu-bar.hpp"
#include "gx-ed-ui-side-panel.hpp"
#include "gx-ed-ui-theme.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

// =====================================================================================
//  Lifecycle
// =====================================================================================

gearoenix::editor::ui::Manager::Manager()
    : Singleton(this)
    , menu_bar(std::make_unique<MenuBar>())
    , entities(std::make_unique<Entities>())
    , components(std::make_unique<Components>())
    , side_panel(std::make_unique<SidePanel>())
    , window_overlay_progress_bar_manager(std::make_unique<WindowOverlayProgressBarManager>())
{
    apply_modern_dark_theme();
    // ImGui context + default font already exist at this point (set up by
    // `BaseApplication::initialise_imgui` before the editor's singletons are
    // constructed), so the Material-Icons TTF can be merged in immediately.
    // If the asset is missing, the load silently fails — icon glyphs will render as
    // tofu but nothing crashes; retrying every frame wouldn't change that.
    (void)load_icon_font();
}

gearoenix::editor::ui::Manager::~Manager() = default;

// =====================================================================================
//  Per-frame tick
// =====================================================================================

void gearoenix::editor::ui::Manager::update()
{
    // The owning `EditorApplication::update()` pushes/pops the UI font size around the
    // whole editor frame, so both UI widgets here and viewport-space overlays (gizmos,
    // toolbar buttons) see a consistent `ImGui::GetFontSize()`.

    menu_bar->update();
    side_panel->update();
    window_overlay_progress_bar_manager->update();
}

void gearoenix::editor::ui::Manager::renew()
{
    menu_bar->renew();
}