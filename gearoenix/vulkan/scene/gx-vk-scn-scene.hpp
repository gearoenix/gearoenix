#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/scene/gx-rnd-scn-scene.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan {
struct DrawState;
}

namespace gearoenix::vulkan::texture {
struct Texture2D;
}

namespace gearoenix::vulkan::scene {
struct Scene final : render::scene::Scene {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_gapi_scene_type_index;
    constexpr static std::array all_parent_object_type_indices { render::scene::Scene::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::scene::Scene::object_type_index };

private:
    std::shared_ptr<texture::Texture2D> brdflut;

    Scene(core::ecs::Entity* e, std::string&& name, core::fp_t layer, std::shared_ptr<render::texture::Texture2D>&& brdflut);
    static void read(
        std::shared_ptr<Scene>&& self,
        std::shared_ptr<platform::stream::Stream>&& stream,
        std::shared_ptr<core::ObjectStreamer>&& object_streamer,
        core::job::EndCaller<>&& end);

public:
    ~Scene() override;
    void render_shadows(DrawState& draw_state);
    void render_reflection_probes(DrawState& draw_state) const;
    void render_forward(DrawState& draw_state);
};
}
#endif