#include "gx-editor-main.hpp"
#include "ui/gx-ed-ui-font.hpp"
#include "ui/gx-ed-ui-manager.hpp"
#include "ui/gx-ed-ui-menu-scene.hpp"
#include "viewport/gx-ed-vp-viewport.hpp"

#include <gearoenix/platform/gx-plt-application.hpp>

#include <ImGui/imgui.h>

#include <memory>

void gearoenix::editor::EditorApplication::update()
{
    // Push the user-selected UI size for the entire editor frame, so UI widgets,
    // viewport buttons and gizmo overlays all see the same scaled font and can
    // derive their geometry from ImGui::GetFontSize().
    ImGui::PushFont(nullptr, ui::font::get_size_px());

    ui_manager->update();
    viewport->update();

    ImGui::PopFont();
}

void gearoenix::editor::EditorApplication::update_caption() const
{
    platform::Application::get().set_caption(std::format("Gearoenix Game Engine Editor [Project: {}, Scene: {}]", current_open_world, current_scene_name));
}

gearoenix::editor::EditorApplication::EditorApplication()
    : Singleton<EditorApplication>(this)
    , ui_manager(std::make_unique<ui::Manager>())
    , viewport(std::make_unique<viewport::Viewport>())
    , current_open_world("Untitled")
{
    update_caption();
}

gearoenix::editor::EditorApplication::~EditorApplication() = default;

void gearoenix::editor::EditorApplication::renew()
{
    ui_manager->renew();
    viewport->renew();
    update_caption();
}

void gearoenix::editor::EditorApplication::quit()
{
    platform::BaseApplication::get().close();
}

void gearoenix::editor::EditorApplication::set_current_scene(const core::ecs::Entity* const scene)
{
    current_scene_name = scene ? scene->get_object_name() : "";
    update_caption();
}

GEAROENIX_START(gearoenix::editor::EditorApplication);