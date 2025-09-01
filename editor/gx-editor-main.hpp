#pragma once
#include <gearoenix/core/ecs/gx-cr-ecs-entity.hpp>
#include <gearoenix/core/gx-cr-application.hpp>

namespace gearoenix::editor::ui {
struct Manager;
}

namespace gearoenix::editor::viewport {
struct Viewport;
}

namespace gearoenix::editor {
struct EditorApplication final : core::Application, core::Singleton<EditorApplication> {
    GX_GET_UPTR_PRV(ui::Manager, ui_manager);
    GX_GET_UPTR_PRV(viewport::Viewport, viewport);
    GX_GET_CREF_PRV(std::string, current_scene_name);
    GX_GET_CREF_PRV(std::string, current_open_world);

    void update() override;
    void update_caption() const;

public:
    EditorApplication();
    ~EditorApplication() override;
    void renew();
    void quit();
    void set_current_scene(const core::ecs::Entity* scene);
};
}