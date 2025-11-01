#include "gx-editor-main.hpp"
#include "ui/gx-ed-ui-manager.hpp"
#include "ui/gx-ed-ui-menu-scene.hpp"
#include "viewport/gx-ed-vp-viewport.hpp"

#include <gearoenix/platform/gx-plt-application.hpp>

void gearoenix::editor::EditorApplication::update()
{
    ui_manager->update();
    viewport->update();
}

void gearoenix::editor::EditorApplication::update_caption() const
{
    platform::Application::get().set_caption(std::format("Gearoenix Game Engine Editor [Project: {}, Scene: {}]", current_open_world, current_scene_name));
}

gearoenix::editor::EditorApplication::EditorApplication()
    : Singleton<EditorApplication>(this)
    , ui_manager(new ui::Manager())
    , viewport(new viewport::Viewport())
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