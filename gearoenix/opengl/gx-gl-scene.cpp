#include "gx-gl-scene.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/gx-cr-profiler.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "gx-gl-camera.hpp"
#include "gx-gl-context.hpp"
#include "gx-gl-label.hpp"
#include "gx-gl-texture.hpp"

#include <ranges>

gearoenix::gl::Scene::Scene(core::ecs::Entity* const entity, std::string&& name, const core::fp_t layer, std::shared_ptr<render::texture::Texture2D>&& brdflut)
    : render::scene::Scene(entity, core::ecs::ComponentType::create_index(this), layer, std::move(name), std::move(brdflut))
{
}

void gearoenix::gl::Scene::read(std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end)
{
    render::scene::Scene::read(std::shared_ptr<render::scene::Scene>(std::move(self)), std::move(stream), std::move(object_streamer), std::move(end));
}

gearoenix::gl::Scene::~Scene() = default;

void gearoenix::gl::Scene::render_shadows(uint& current_shader)
{
    GX_GL_PUSH_DEBUG_GROUP("Shadow pass of Scene {}", object_name);
    for (const auto& index : record.cameras.shadow_casters | std::views::values) {
        auto& cmr_rcd = record.cameras.cameras[index];
        GX_PROFILE_EXP(core::cast_ptr<Camera>(cmr_rcd.camera)->render_shadow(cmr_rcd, current_shader));
    }
}

void gearoenix::gl::Scene::render_reflection_probes(uint& current_shader) const
{
    GX_GL_PUSH_DEBUG_GROUP("Reflection probe pass of Scene: {}", object_name);
    for (const auto ci : record.cameras.reflections | std::views::values) {
        auto& camera = record.cameras.cameras[ci];
        GX_PROFILE_EXP(core::cast_ptr<Camera>(camera.camera)->render_forward(*this, camera, current_shader));
    }
}

void gearoenix::gl::Scene::render_forward(uint& current_shader)
{
    render_reflection_probes(current_shader);
    for (const auto camera_index : record.cameras.mains | std::views::values) {
        auto& camera = record.cameras.cameras[camera_index];
        auto* const gl_cam = core::cast_ptr<Camera>(camera.camera);
        GX_PROFILE_EXP(gl_cam->render_forward(*this, camera, current_shader));
        GX_PROFILE_EXP(gl_cam->render_bloom(*this, camera, current_shader));
        GX_PROFILE_EXP(gl_cam->render_colour_tuning(*this, camera, current_shader));
    }
}

gearoenix::gl::SceneManager::SceneManager()
{
    core::ecs::ComponentType::add<Scene>();
}

gearoenix::gl::SceneManager::~SceneManager() = default;

void gearoenix::gl::SceneManager::build(std::string&& name, const core::fp_t layer, core::job::EndCaller<core::ecs::EntityPtr>&& end) const
{
    auto entity = core::ecs::Entity::construct(std::move(name), nullptr);
    end.set_return(std::move(entity));
    Singleton<TextureManager>::get().get_brdflut(core::job::EndCallerShared<render::texture::Texture2D>([end = std::move(end), layer](std::shared_ptr<render::texture::Texture2D>&& t) {
        auto* const e = end.get_return().get();
        e->add_component(core::Object::construct<Scene>(e, e->get_object_name() + "-scene", layer, std::move(t)));
    }));
}

#endif
