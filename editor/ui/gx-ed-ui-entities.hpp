#pragma once
#include "gx-ed-ui-dockable-section.hpp"

#include <gearoenix/core/gx-cr-singleton.hpp>

#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::editor::ui {
/// Scene hierarchy view: a searchable tree of active scenes and their
/// descendants. Each row shows a coloured icon and the entity name; clicking a row sets
/// the "current selection" that the Components panel then renders.
///
/// Selection is held as a `weak_ptr` so deletions in the ECS world transparently clear
/// the highlighted entity without dangling-pointer risk.
///
/// Implements `DockableSection` so a host container (the editor's `SidePanel`) can dock
/// or float it without knowing about the scene-hierarchy specifics.
struct Entities final : core::Singleton<Entities>, DockableSection {
private:
    std::weak_ptr<core::ecs::Entity> selected;
    std::string search_filter;

    void draw_entity_row(const core::ecs::Entity* entity, int depth);
    [[nodiscard]] bool matches_filter(const core::ecs::Entity* entity) const;

public:
    Entities();
    ~Entities() override;

    // -- DockableSection ---------------------------------------------------------------
    /// Renders the search/filter input inline in the section header.
    void draw_header_inline_content(float avail_w) override;
    /// Render the entity tree into the current ImGui host window.
    void update() override;

    /// Replace the current selection (pass `nullptr` to clear).
    void set_selected(core::ecs::Entity* entity);

    /// @return the currently selected entity or `nullptr` if none or the previous selection
    ///          has been destroyed.
    [[nodiscard]] core::ecs::Entity* get_selected() const;
};
}