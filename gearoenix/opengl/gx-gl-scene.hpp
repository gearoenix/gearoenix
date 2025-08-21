#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/scene/gx-rnd-scn-manager.hpp"
#include "../render/scene/gx-rnd-scn-scene.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Scene final : render::scene::Scene {
    constexpr static auto object_type_index = gearoenix_gl_scene_type_index;
    constexpr static std::array all_parent_object_type_indices { render::scene::Scene::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::scene::Scene::object_type_index };

    Scene(core::ecs::Entity* entity, std::string&& name, double layer);
    ~Scene() override;
    void update() override;
    void render_shadows(uint& current_shader);
    void render_reflection_probes(uint& current_shader) const;
    void render_forward(uint& current_shader);
};

struct SceneManager final : render::scene::Manager {
    SceneManager();
    ~SceneManager() override;
    [[nodiscard]] core::ecs::EntityPtr build(std::string&& name, double layer) const override;
};
}

#endif