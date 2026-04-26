#pragma once

#include <ImGui/imgui.h>

#include <string>
#include <utility>

namespace gearoenix::editor::ui {

/// Abstract base for any section that a `SidePanel` (or any other host container) can
/// render. The host draws the chrome around the section — frame, header, dock-toggle
/// button, floating-window wrapper — and delegates the body to `update()`.
///
/// Each section can live inside a host panel ("docked") or as a free-floating ImGui
/// window. The dock / float / hide state is held as plain data members here (rather
/// than in a separate struct exposed via a getter) because it's intrinsic to the
/// abstraction — every dockable section *has* this state.
///
/// @field title             human-readable title shown in the section header and the
///                          floating-window title bar. Passed in by the subclass via the
///                          base constructor; subclasses may mutate it later for dynamic
///                          titles ("Entities (3)") without paying a per-frame alloc.
/// @field shown             whether the section is visible anywhere (toggled by the Window menu).
/// @field attached          docked into the host side panel (true) or floating (false).
/// @field floating_pos/size the last known geometry of the floating window. Applied with
///                          `ImGuiCond_Appearing` so ImGui pushes them only when the
///                          window transitions are hidden → visible (i.e. on each dock → float),
///                          and lets the user drag / resize freely in between. The host
///                          reads `GetWindowPos` / `GetWindowSize` back each frame so the
///                          stored values stay in sync with whatever the user did.
struct DockableSection {
    std::string title;
    bool shown = true;
    bool attached = true;
    ImVec2 floating_pos { 120.0f, 120.0f };
    ImVec2 floating_size { 360.0f, 420.0f };

    explicit DockableSection(std::string&& title)
        : title(std::move(title))
    {
    }

    virtual ~DockableSection() = default;
    DockableSection(DockableSection&&) = delete;
    DockableSection(const DockableSection&) = delete;
    DockableSection& operator=(DockableSection&&) = delete;
    DockableSection& operator=(const DockableSection&) = delete;

    /// Optional per-section content drawn inline on the section's header row, between
    /// the title (left) and the dock-toggle button (right). The host has already placed
    /// the cursor after the title; the section gets `avail_w` pixels of width to fill
    /// and is responsible for constraining its own widgets (e.g. via `SetNextItemWidth`).
    /// Default: no-op — sections that don't contribute inline content leave the space
    /// empty, and the dock button hugs the right edge.
    virtual void draw_header_inline_content(float /*avail_w*/) { }

    /// Render the section body into the currently active ImGui host window (either the
    /// docked child panel or the floating top-level window — the choice belongs to the host).
    virtual void update() = 0;

    /// True when the section is shown AND attached — i.e. it should appear inside the
    /// host's docked panel this frame. Sections that are hidden, or popped out into a
    /// floating window, return false here.
    [[nodiscard]] bool is_docked() const { return shown && attached; }

    /// Render the section header row — title on the left, the section's inline content
    /// in the middle (`draw_header_inline_content`), and the dock-toggle icon button on
    /// the right. `avail_w` is the row's available width as decided by the host.
    void draw_header(float avail_w);

    /// Render the standard "header + separator + body" stack into the current ImGui host
    /// window (a child panel for docked sections, a top-level window for floating ones).
    /// Available width is read from the current ImGui content region.
    void draw_docked_body();

    /// Render this section as a free-floating ImGui top-level window. No-op when the
    /// section is hidden or attached (it belongs in the docked panel in those cases).
    /// Updates `floating_pos` / `floating_size` from ImGui each frame so subsequent
    /// re-pop-outs resume where the user left off, and propagates the window's close-X
    /// click back into `shown`.
    void draw_as_floating_window();
};
}