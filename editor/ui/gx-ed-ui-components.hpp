#pragma once
#include "gx-ed-ui-dockable-section.hpp"

#include <gearoenix/core/gx-cr-singleton.hpp>

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::editor::ui {
/// Components panel: driven by whatever entity the Entities panel
/// currently has selected, it renders a header with the entity's primary icon + name
/// followed by a tab bar — one tab per component — whose body hosts that component's
/// existing `show_debug_gui()` implementation.
///
/// Implements `DockableSection` so a host container (the editor's `SidePanel`) can
/// dock or float it without knowing about the component-panel specifics.
struct Components final : core::Singleton<Components>, DockableSection {
    Components();
    ~Components() override;

    /// Renders the selected entity's icon + name inline in the section header.
    void draw_header_inline_content(float avail_w) override;
    /// Render the component tabs into the current ImGui host window.
    void update() override;
};
}