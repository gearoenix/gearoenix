#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/scene/gx-rnd-scn-manager.hpp"
#include "../render/scene/gx-rnd-scn-scene.hpp"
#include "submission/gx-gl-sbm-model.hpp"

namespace gearoenix::gl {
struct Scene final : render::scene::Scene {
    constexpr static auto object_type_index = gearoenix_gl_scene_type_index;
    constexpr static std::array all_parent_object_type_indices { render::scene::Scene::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::scene::Scene::object_type_index };

    explicit Scene(std::string&& name, double layer);
    ~Scene() override;
    void update() override;
};

struct SceneManager final : render::scene::Manager {
    SceneManager();
    ~SceneManager() override;
    [[nodiscard]] core::ecs::EntityPtr build(std::string&& name, double layer) const override;
};
}

#endif