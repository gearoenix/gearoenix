#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "../render/record/gx-rnd-rcd-camera.hpp"
#include "../render/record/gx-rnd-rcd-model.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"
#include "gx-gl-label.hpp"
#include "gx-gl-context.hpp"
#include "gx-gl-model.hpp"
#include "gx-gl-scene.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-skybox.hpp"
#include "boost/mp11/algorithm.hpp"
#include "shader/gx-gl-shd-manager.hpp"
#include "shader/gx-gl-shd-skybox-cube.hpp"
#include "shader/gx-gl-shd-skybox-equirectangular.hpp"

gearoenix::gl::CameraTarget::~CameraTarget() = default;

gearoenix::gl::CameraTarget gearoenix::gl::CameraTarget::construct(const render::camera::Target& target)
{
    if (target.is_default()) {
        const auto& d = target.get_default();
        CameraTarget result { .target = Default {} };
        auto& gd = std::get<default_var_index>(result.target);
        gd.main = std::dynamic_pointer_cast<Target>(d.main);
        for (std::uint32_t ti = 0; ti < d.targets.size(); ++ti) {
            for (std::uint32_t mi = 0; mi < d.targets[ti].size(); ++mi) {
                gd.targets[ti][mi] = std::dynamic_pointer_cast<Target>(d.targets[ti][mi]);
            }
        }
        return result;
    }
    if (target.is_customised()) {
        const auto& d = target.get_customised();
        return CameraTarget { .target = Customised { .target = std::dynamic_pointer_cast<Target>(d.target) } };
    }
    GX_UNEXPECTED;
}

const gearoenix::gl::CameraTarget::Customised& gearoenix::gl::CameraTarget::get_customised() const
{
    GX_ASSERT(customised_var_index == target.index());
    return std::get<customised_var_index>(target);
}

const gearoenix::gl::CameraTarget::Default& gearoenix::gl::CameraTarget::get_default() const
{
    GX_ASSERT(default_var_index == target.index());
    return std::get<default_var_index>(target);
}

void gearoenix::gl::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gl_target.target = CameraTarget::Customised { .target = std::dynamic_pointer_cast<Target>(t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::gl::Camera::update_target(core::job::EndCaller<>&& end)
{
    render::camera::Camera::update_target(core::job::EndCaller([this, self = object_self.lock(), end = std::move(end)] {
        (void)end;
        (void)self;
        gl_target = CameraTarget::construct(target);
    }));
}

gearoenix::gl::Camera::Camera(const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform)
    : render::camera::Camera(core::ecs::ComponentType::create_index(this), name, std::move(target), std::move(transform))
    , skybox_cube_shader(shader::Manager::get_shader<shader::SkyboxCube>())
    , skybox_equirectangular_shader(shader::Manager::get_shader<shader::SkyboxEquirectangular>())
{
}

void gearoenix::gl::Camera::construct(const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform)
{
    c.set_return(Object::construct<Camera>(name, render::camera::Target(), std::move(transform)));
    c.get_return()->initialise();
    c.get_return()->update_target(core::job::EndCaller([c] {
        c.get_return()->enable_bloom();
    }));
}

gearoenix::gl::Camera::~Camera() = default;

void gearoenix::gl::Camera::render_shadow(const render::record::Camera& cmr, uint& current_shader)
{
    push_debug_group(render_pass_name);
    ctx::set_framebuffer(gl_target.get_customised().target->get_framebuffer());
    ctx::set_viewport_scissor_clip(math::Vec4<sizei>(cmr.viewport_clip));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    for (auto& distance_model_data : cmr.all_models) {
        auto& camera_model = distance_model_data.second;
        auto& model = *static_cast<Model*>(camera_model.model->model);
        model.render_shadow(cmr, camera_model, current_shader);
    }
    pop_debug_group();
}

void gearoenix::gl::Camera::render_forward(const Scene& scene, const render::record::Camera& cmr, uint& current_shader)
{
    static std::string debug_group;
    debug_group.clear();
    debug_group += "render-forward-camera for scene: ";
    debug_group += scene.get_object_name();
    debug_group += ", and for camera: ";
    debug_group += object_name;

    push_debug_group( debug_group);
    GX_GL_CHECK_D;
    if (target.is_customised())
    {
        ctx::set_framebuffer(gl_target.get_customised().target->get_framebuffer());
    } else
    {
        ctx::set_framebuffer(gl_target.get_default().main->get_framebuffer());
    }
    ctx::set_viewport_scissor_clip(math::Vec4<sizei>(cmr.viewport_clip));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    render_forward_skyboxes(scene, cmr, current_shader);
    GX_GL_CHECK_D;
    glEnable(GL_BLEND); /// TODO: take it into a context and material must decide
    // Rendering forward pbr
    for (auto& distance_model_data : cmr.all_models) {
        auto& camera_model = distance_model_data.second;
        auto& model = *static_cast<Model*>(camera_model.model->model);
        model.render_forward(scene, cmr, camera_model, current_shader);
        GX_GL_CHECK_D;
    }
    glDisable(GL_BLEND); /// TODO: take it into a context and material must decide
    pop_debug_group();
}

void gearoenix::gl::Camera::render_forward_skyboxes(const Scene& scene, const render::record::Camera& cmr, uint& current_shader)
{
    static std::string debug_group;
    debug_group.clear();
    debug_group += "render-skyboxes for scene: ";
    debug_group += scene.get_object_name();
    debug_group += ", and for camera: ";
    debug_group += object_name;

    push_debug_group( debug_group);
    glDepthMask(GL_FALSE);
    // Rendering skyboxes
    const auto camera_pos_scale = math::Vec4(cmr.position, cmr.skybox_scale);
    bool is_equirectangular_current = true;
    skybox_equirectangular_shader->bind(current_shader);
    skybox_equirectangular_shader->set_vp_data(cmr.view_projection.data());
    skybox_equirectangular_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
    auto skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
    for (const auto& distance_skybox : scene.get_record().skyboxes.skyboxes) {
        const auto& skybox = distance_skybox.second;
        const auto is_equirectangular = skybox.equirectangular;
        if (is_equirectangular != is_equirectangular_current) {
            is_equirectangular_current = is_equirectangular;
            if (is_equirectangular) {
                skybox_equirectangular_shader->bind(current_shader);
                skybox_equirectangular_shader->set_vp_data(cmr.view_projection.data());
                skybox_equirectangular_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
                skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
            } else {
                skybox_cube_shader->bind(current_shader);
                skybox_cube_shader->set_vp_data(cmr.view_projection.data());
                skybox_cube_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
                skybox_texture_bind_index = static_cast<enumerated>(skybox_cube_shader->get_albedo_index());
            }
        }
        glActiveTexture(GL_TEXTURE0 + skybox_texture_bind_index);
        const auto& gl_sky = *static_cast<const Skybox*>(skybox.skybox);
        const auto& gl_txt = gl_sky.get_gl_texture();
        constexpr auto texture_2d_index = boost::mp11::mp_find<Skybox::GlTexture, std::shared_ptr<Texture2D>>::value;
        constexpr auto texture_cube_index = boost::mp11::mp_find<Skybox::GlTexture, std::shared_ptr<TextureCube>>::value;
        if (is_equirectangular)
        {
            glBindTexture(GL_TEXTURE_2D, std::get<texture_2d_index>(gl_txt)->get_object());
        } else
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, std::get<texture_cube_index>(gl_txt)->get_object());
        }
        glBindVertexArray(gl_sky.get_vertex_object());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }
    glDepthMask(GL_TRUE);
    pop_debug_group();
}

void gearoenix::gl::CameraManager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    build_impl(std::move(name), parent, entity_callback);
    const auto* const entity = entity_callback.get_return().get();
    auto transform = entity->get_component_shared_ptr<physics::Transformation>();
    Camera::construct(
        entity->get_object_name() + "-gl-camera",
        core::job::EndCallerShared<Camera>([end = std::move(entity_callback)](std::shared_ptr<Camera>&& c) {
            end.get_return()->add_component(std::move(c));
        }),
        std::move(transform));
}

gearoenix::gl::CameraManager::CameraManager()
{
    core::ecs::ComponentType::add<Camera>();
}

void gearoenix::gl::CameraManager::window_resized()
{
    Manager::window_resized();
}

gearoenix::gl::CameraManager::~CameraManager() = default;

#endif
