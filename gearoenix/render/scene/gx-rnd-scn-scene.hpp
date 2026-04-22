#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "../buffer/gx-rnd-buf-uniform.hpp"
#include "../record/gx-rnd-rcd-scene.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::scene {
struct Scene : core::ecs::Component {
    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_render_scene_type_index;

    // radius, normal-jitter, min and max depth values for occlusion
    GX_GET_REF_PRT(math::Vec4<float>, ssao_settings);
    GX_GET_REF_PRT(math::Vec3<float>, ambient_light);
    GX_GET_REF_PRT(record::Scene, record);
    GX_GETSET_VAL_PRT(core::fp_t, layer, 0.0);
    GX_GETSET_VAL_PRT(bool, changed, true);
    GX_GET_CREF_PRT(buffer::Uniform, uniform);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, brdflut);

    Scene(core::ecs::Entity* entity, core::object_type_index_t final_type_index, core::fp_t layer, std::string&& name, std::shared_ptr<texture::Texture2D>&& brdflut);
    static void read(std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end);
    void write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end_caller) override;

public:
    ~Scene() override;
    void update_uniform();
    virtual void update_per_frame();

    /// Update the whole scene after major changes
    ///
    /// This function updates the internal structure of the scene after any major change in the scene.
    /// After these changes you need to call this function:
    ///   * - Any child-entity addition to or deletion from a scene.
    ///   * - Any transformation in static entities (not the dynamic entities).
    ///   * - Any update in enable/disable of any child-entity.
    ///   * - Look into the implementation for further info and update this list.
    /// @note This is an expensive operation, try to avoid calling it too often and try to batch your changes.
    virtual void update_after_change();
};
}
