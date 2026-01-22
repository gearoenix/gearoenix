#include "gx-gl-scene.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "gx-gl-camera.hpp"
#include "gx-gl-context.hpp"
#include "gx-gl-label.hpp"

#include <ranges>

gearoenix::gl::Scene::Scene(core::ecs::Entity* const entity, std::string&& name, const double layer)
    : render::scene::Scene(entity, core::ecs::ComponentType::create_index(this), layer, std::move(name))
{
}

gearoenix::gl::Scene::Scene(const core::object_id_t id, std::string&& name)
    : render::scene::Scene(core::ecs::ComponentType::create_index(this), id, std::move(name))
{
}

void gearoenix::gl::Scene::read(
    std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream,
    std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end)
{
    render::scene::Scene::read(std::shared_ptr<render::scene::Scene>(std::move(self)), std::move(stream), std::move(object_streamer), std::move(end));
}

gearoenix::gl::Scene::~Scene() = default;

void gearoenix::gl::Scene::update()
{
    render::scene::Scene::update();
}

void gearoenix::gl::Scene::render_shadows(uint& current_shader)
{
    push_debug_group(shadow_render_pass_name);
    for (const auto& index : record.cameras.shadow_casters | std::views::values) {
        auto& cmr_rcd = record.cameras.cameras[index];
        core::cast_ptr<Camera>(cmr_rcd.camera)->render_shadow(cmr_rcd, current_shader);
    }
    pop_debug_group();
}

void gearoenix::gl::Scene::render_reflection_probes(uint& current_shader) const
{
    push_debug_group(shadow_reflection_probe_render_pass_name);
    for (const auto ci : record.cameras.reflections | std::views::values) {
        auto& camera = record.cameras.cameras[ci];
        core::cast_ptr<Camera>(camera.camera)->render_forward(*this, camera, current_shader);
    }
    pop_debug_group();
}

void gearoenix::gl::Scene::render_forward(uint& current_shader)
{
    render_reflection_probes(current_shader);
    for (const auto camera_index : record.cameras.mains | std::views::values) {
        auto& camera = record.cameras.cameras[camera_index];
        auto* const gl_cam = core::cast_ptr<Camera>(camera.camera);
        gl_cam->render_forward(*this, camera, current_shader);
        gl_cam->render_bloom(*this, camera, current_shader);
        gl_cam->render_colour_correction_anti_aliasing(*this, camera, current_shader);
    }
}

gearoenix::gl::SceneManager::SceneManager()
{
    core::ecs::ComponentType::add<Scene>();
}

gearoenix::gl::SceneManager::~SceneManager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::gl::SceneManager::build(std::string&& name, double layer) const
{
    auto entity = Manager::build(std::move(name), layer);
    entity->add_component(core::Object::construct<Scene>(entity.get(), entity->get_object_name() + "-scene", layer));
    return entity;
}

#endif
