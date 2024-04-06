#include "gx-rnd-scn-splash.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../font/gx-rnd-fnt-font.hpp"
#include "../font/gx-rnd-fnt-manager.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-scn-builder.hpp"
#include "gx-rnd-scn-manager.hpp"
#include "gx-rnd-scn-scene.hpp"

constexpr double gear_base_scale = 0.125;
constexpr double glare_base_scale = 0.15;
constexpr double wings_base_scale = 0.15;
constexpr double gearoenix_text_base_scale = 0.0375;
constexpr double animation_speed = 1.2;

constexpr double whole_scale_animation = 0.5;
constexpr double whole_scale_start = 1.0 - whole_scale_animation;

constexpr double wing_angle_start = -0.4;
constexpr double wing_angle_end = 0.2;
constexpr double wing_rotate_dis = wing_angle_end - wing_angle_start;

double gearoenix::render::scene::Splash::calculate_scale() const
{
    auto& win_sz = e.get_platform_application().get_base().get_window_size();
    const auto win_ar = static_cast<double>(win_sz.x) / static_cast<double>(win_sz.y);
    constexpr auto threshold = 1.5;
    constexpr auto out = 1.3;
    if (win_ar > threshold)
        return out;
    return win_ar * (out / threshold);
}

gearoenix::render::scene::Splash::Splash(
    engine::Engine& e,
    const core::job::EndCaller<>& end_callback)
    : e(e)
    , end_callback(end_callback)
    , current_scale(calculate_scale() * whole_scale_start)
    , wings_current_angle(wing_angle_start)
{
}

void gearoenix::render::scene::Splash::initialise(const core::job::EndCaller<>& start_callback)
{
    std::vector<gearoenix::render::PbrVertex> plane_vertices(4);
    plane_vertices[0].position = { 1.0f, 1.0f, 0.0f };
    plane_vertices[0].uv = { 1.0f, 0.0f };
    plane_vertices[1].position = { -1.0f, 1.0f, 0.0f };
    plane_vertices[1].uv = { 0.0f, 0.0f };
    plane_vertices[2].position = { 1.0f, -1.0f, 0.0f };
    plane_vertices[2].uv = { 1.0f, 1.0f };
    plane_vertices[3].position = { -1.0f, -1.0f, 0.0f };
    plane_vertices[3].uv = { 0.0f, 1.0f };

    auto left_wing_vertices = plane_vertices;
    for (auto& v : left_wing_vertices) {
        v.position += math::Vec3(1.3f, 0.8f, 0.0f);
        v.uv.y = std::abs(v.uv.y - 1.0f);
    }

    auto right_wing_vertices = plane_vertices;
    for (auto& v : right_wing_vertices) {
        v.position -= math::Vec3(1.3f, -0.8f, 0.0f);
        v.uv.x = std::abs(v.uv.x - 1.0f);
        v.uv.y = std::abs(v.uv.y - 1.0f);
    }

    std::vector<std::uint32_t> plane_indices = { 0, 1, 3, 0, 3, 2 };
    auto left_wing_indices = plane_indices;
    auto right_wing_indices = plane_indices;

    auto entity_callback = core::job::EndCaller([this, end = start_callback]() -> void {
        this->e.get_world()->get_component<Scene>(scene_id)->set_enabled(true);
    });

    auto scene_builder = e.get_scene_manager()->build(
        "gearoenix-splash-scene", 0.0, core::job::EndCaller(entity_callback));
    scene_id = scene_builder->get_entity_builder()->get_builder().get_id();

    {
        e.get_camera_manager()->build(
            "gearoenix-splash-camera",
            core::job::EndCallerShared<camera::Builder>([scene_builder](std::shared_ptr<camera::Builder>&& cb) {
                cb->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });
                auto& camera = cb->get_camera();
                camera.set_projection_data(camera::OrthographicProjectionData { .scale = 1.0 });
                camera.disable_bloom();
                camera.get_colour_tuning() = camera::RawColour {};
                scene_builder->add(std::move(cb));
            }),
            core::job::EndCaller(entity_callback));
    }

    struct Values {
        engine::Engine& e;
        std::shared_ptr<scene::Builder> scene_builder;
        core::job::EndCaller<> entity_callback;
        std::shared_ptr<Splash> splash;

        std::shared_ptr<mesh::Mesh> plane_mesh;
        std::shared_ptr<mesh::Mesh> plate_mesh;
        std::shared_ptr<mesh::Mesh> left_wing_mesh;
        std::shared_ptr<mesh::Mesh> right_wing_mesh;
        std::shared_ptr<material::Unlit> bg_mat;
        std::shared_ptr<material::Unlit> gear_mat;
        std::shared_ptr<material::Unlit> glare_mat;
        std::shared_ptr<material::Unlit> wing_mat;
        std::shared_ptr<material::Unlit> text_mat;
        std::shared_ptr<texture::Texture2D> gear_txt;
        std::shared_ptr<texture::Texture2D> glare_txt;
        std::shared_ptr<texture::Texture2D> wing_txt;
        std::shared_ptr<texture::Texture2D> text_txt;
        double text_width = 0.0, text_height = 0.0;

        Values(
            engine::Engine& e,
            std::shared_ptr<Splash>&& splash,
            std::shared_ptr<scene::Builder>&& scene_builder,
            core::job::EndCaller<>&& entity_callback)
            : e(e)
            , scene_builder(std::move(scene_builder))
            , entity_callback(std::move(entity_callback))
            , splash(std::move(splash))
        {
        }

        ~Values()
        {
            {
                auto bg_model_builder = e.get_model_manager()->build(
                    "gearoenix-splash-bg",
                    std::shared_ptr(plane_mesh),
                    std::move(bg_mat),
                    core::job::EndCaller(entity_callback),
                    true);
                bg_model_builder->get_transformation().local_scale(100.0);
                scene_builder->add(std::move(bg_model_builder));
            }

            {
                gear_mat->set_transparency(material::Transparency::Transparent);
                gear_mat->set_albedo(std::move(gear_txt));
                auto gear_model_builder = e.get_model_manager()->build(
                    "gearoenix-splash-gear",
                    std::shared_ptr(plane_mesh),
                    std::move(gear_mat),
                    core::job::EndCaller(entity_callback),
                    true);
                gear_model_builder->get_transformation().local_scale(splash->current_scale * gear_base_scale);
                gear_model_builder->get_transformation().local_translate({ 0.0f, 0.0f, 0.3f });
                splash->gear_id = gear_model_builder->get_entity_builder()->get_builder().get_id();
                scene_builder->add(std::move(gear_model_builder));
            }

            {
                glare_mat->set_transparency(material::Transparency::Transparent);
                glare_mat->set_albedo(std::move(glare_txt));
                auto glare_model_builder = e.get_model_manager()->build(
                    "gearoenix-splash-glare",
                    std::shared_ptr(plane_mesh),
                    std::move(glare_mat),
                    core::job::EndCaller(entity_callback),
                    true);
                glare_model_builder->get_transformation().local_scale(splash->current_scale * glare_base_scale);
                glare_model_builder->get_transformation().local_translate({ 0.0f, 0.0f, 0.2f });
                splash->glare_id = glare_model_builder->get_entity_builder()->get_builder().get_id();
                scene_builder->add(std::move(glare_model_builder));
            }

            wing_mat->set_transparency(material::Transparency::Transparent);
            wing_mat->set_albedo(std::move(wing_txt));

            {
                auto wing_model_builder = e.get_model_manager()->build(
                    "gearoenix-splash-left-wing",
                    std::move(left_wing_mesh),
                    std::shared_ptr(wing_mat),
                    core::job::EndCaller(entity_callback),
                    true);
                wing_model_builder->get_transformation().local_scale(splash->current_scale * wings_base_scale);
                wing_model_builder->get_transformation().local_translate({ 0.0, 0.0, 0.1 });
                wing_model_builder->get_transformation().local_z_rotate(-wing_angle_start);
                splash->left_wing_id = wing_model_builder->get_entity_builder()->get_builder().get_id();
                scene_builder->add(std::move(wing_model_builder));
            }

            {
                auto wing_model_builder = e.get_model_manager()->build(
                    "gearoenix-splash-right-wing",
                    std::move(right_wing_mesh),
                    std::move(wing_mat),
                    core::job::EndCaller(entity_callback),
                    true);
                wing_model_builder->get_transformation().local_scale(splash->current_scale * wings_base_scale);
                wing_model_builder->get_transformation().local_translate({ 0.0f, 0.0f, 0.1f });
                wing_model_builder->get_transformation().local_z_rotate(wing_angle_start);
                splash->right_wing_id = wing_model_builder->get_entity_builder()->get_builder().get_id();
                scene_builder->add(std::move(wing_model_builder));
            }

            {
                text_mat->set_transparency(material::Transparency::Transparent);
                text_mat->get_albedo_factor() = { 0.4f, 0.0f, 0.0f, 0.0f };
                math::Vec3<double> scale(splash->current_scale * gearoenix_text_base_scale);
                scale.x *= text_width / text_height;
                text_mat->set_albedo(std::move(text_txt));
                auto model_builder = e.get_model_manager()->build(
                    "gearoenix-splash-text",
                    std::move(plate_mesh),
                    std::move(text_mat),
                    std::move(entity_callback),
                    true);
                auto& trn = model_builder->get_transformation();
                trn.local_scale(scale);
                trn.local_translate({ 0.0f, -0.25f, 0.5f });
                splash->gearoenix_text = model_builder->get_id();
                scene_builder->add(std::move(model_builder));
            }
        }
    };

    auto values = std::make_shared<Values>(e, weak_self.lock(), std::move(scene_builder), std::move(entity_callback));

    const auto font = e.get_font_manager()->get("Roboto-Bold");

    e.get_mesh_manager()->build(
        "gearoenix-splash-plane",
        std::move(plane_vertices),
        std::move(plane_indices),
        core::job::EndCallerShared<mesh::Mesh>([values](std::shared_ptr<mesh::Mesh>&& m) {
            values->plane_mesh = std::move(m);
        }));

    e.get_mesh_manager()->build(
        "gearoenix-splash-left-wing",
        std::move(left_wing_vertices),
        std::move(left_wing_indices),
        core::job::EndCallerShared<mesh::Mesh>([values](std::shared_ptr<mesh::Mesh>&& m) {
            values->left_wing_mesh = std::move(m);
        }));

    e.get_mesh_manager()->build(
        "gearoenix-splash-right-wing",
        std::move(right_wing_vertices),
        std::move(right_wing_indices),
        core::job::EndCallerShared<mesh::Mesh>([values](std::shared_ptr<mesh::Mesh>&& m) {
            values->right_wing_mesh = std::move(m);
        }));

    e.get_mesh_manager()->build_plate(
        core::job::EndCallerShared<mesh::Mesh>([values](std::shared_ptr<mesh::Mesh>&& m) {
            values->plate_mesh = std::move(m);
        }));

    e.get_material_manager()->get_unlit(
        "gearoenix-splash-bg",
        core::job::EndCallerShared<material::Unlit>([values](std::shared_ptr<material::Unlit>&& m) {
            values->bg_mat = std::move(m);
        }));

    e.get_material_manager()->get_unlit(
        "gearoenix-splash-gear",
        core::job::EndCallerShared<material::Unlit>([values](std::shared_ptr<material::Unlit>&& m) {
            values->gear_mat = std::move(m);
        }));

    e.get_material_manager()->get_unlit(
        "gearoenix-splash-glare",
        core::job::EndCallerShared<material::Unlit>([values](std::shared_ptr<material::Unlit>&& m) {
            values->glare_mat = std::move(m);
        }));

    e.get_material_manager()->get_unlit(
        "gearoenix-splash-wing",
        core::job::EndCallerShared<material::Unlit>([values](std::shared_ptr<material::Unlit>&& m) {
            values->wing_mat = std::move(m);
        }));

    e.get_material_manager()->get_unlit(
        "gearoenix-splash-text",
        core::job::EndCallerShared<material::Unlit>([values](std::shared_ptr<material::Unlit>&& m) {
            values->text_mat = std::move(m);
        }));

    e.get_texture_manager()->create_2d_from_file(
        "gearoenix-splash-gear",
        platform::stream::Path::create_asset("gearoenix-splash/gear.png"),
        texture::TextureInfo {},
        core::job::EndCallerShared<texture::Texture2D>([values](std::shared_ptr<texture::Texture2D>&& t) {
            values->gear_txt = std::move(t);
        }));

    e.get_texture_manager()->create_2d_from_file(
        "gearoenix-splash-glare",
        platform::stream::Path::create_asset("gearoenix-splash/glare.png"),
        texture::TextureInfo(),
        core::job::EndCallerShared<texture::Texture2D>([values](std::shared_ptr<texture::Texture2D>&& t) {
            values->glare_txt = std::move(t);
        }));

    e.get_texture_manager()->create_2d_from_file(
        "gearoenix-splash-wing",
        gearoenix::platform::stream::Path::create_asset("gearoenix-splash/wing.png"),
        gearoenix::render::texture::TextureInfo(),
        core::job::EndCallerShared<texture::Texture2D>([values](std::shared_ptr<texture::Texture2D>&& t) {
            values->wing_txt = std::move(t);
        }));

    values->text_height = e.get_platform_application().get_base().get_screen_size().y * 0.0001;
    font->bake(
        L"Gearoenix", math::Vec4(1.0), values->text_height, values->text_width,
        core::job::EndCallerShared<texture::Texture2D>([values](std::shared_ptr<texture::Texture2D>&& t) {
            values->text_txt = std::move(t);
        }));
}

std::shared_ptr<gearoenix::render::scene::Splash> gearoenix::render::scene::Splash::construct(
    engine::Engine& e,
    const core::job::EndCaller<>& start_callback,
    const core::job::EndCaller<>& end_callback)
{
    std::shared_ptr<Splash> result(new Splash(e, end_callback));
    result->weak_self = result;
    result->initialise(start_callback);
    return result;
}

void gearoenix::render::scene::Splash::update()
{
    auto* const gear_tran = e.get_world()->get_component<physics::Transformation>(gear_id);
    if (gear_tran == nullptr)
        return;
    auto* const glare_tran = e.get_world()->get_component<physics::Transformation>(glare_id);
    if (glare_tran == nullptr)
        return;
    auto* const left_wing_tran = e.get_world()->get_component<physics::Transformation>(left_wing_id);
    if (left_wing_tran == nullptr)
        return;
    auto* const right_wing_tran = e.get_world()->get_component<physics::Transformation>(right_wing_id);
    if (right_wing_tran == nullptr)
        return;
    auto* const gearoenix_tran = e.get_world()->get_component<physics::Transformation>(gearoenix_text);
    if (gearoenix_tran == nullptr)
        return;
    auto* const gearoenix_model = e.get_world()->get_component<model::Model>(gearoenix_text);
    if (gearoenix_model == nullptr)
        return;

    const auto delta_time = e.get_delta_time() > 0.05 ? 0.05 : e.get_delta_time();

    if (is_animating_scale) {
        scale_animation_time_current += delta_time * animation_speed;
        if (scale_animation_time_current > 1.0) {
            scale_animation_time_current = 1.0;
            is_animating_scale = false;
            is_animating_text = true;
        }
    } else if (is_animating_text) {
        text_animation_time_current += delta_time * animation_speed;
        if (text_animation_time_current > 1.0) {
            text_animation_time_current = 1.0;
            is_animating_text = false;
        }
        dynamic_cast<material::Unlit*>(gearoenix_model->get_bound_material().get())->get_albedo_factor().w = static_cast<float>(text_animation_time_current);
    } else {
        after_animation += delta_time;
        if (after_animation > 1.0) {
            end_callback = std::nullopt;
            (void)end_callback;
        }
    }

    gear_tran->local_z_rotate(delta_time);
    glare_tran->local_z_rotate(-delta_time);

    const auto new_scale = calculate_scale() * (whole_scale_start + whole_scale_animation * scale_animation_time_current);
    const auto scale = new_scale / current_scale;
    current_scale = new_scale;

    gear_tran->local_scale(scale);
    glare_tran->local_scale(scale);
    left_wing_tran->local_scale(scale);
    right_wing_tran->local_scale(scale);
    gearoenix_tran->local_scale(scale);

    const auto new_wing_angle = wing_angle_start + scale_animation_time_current * wing_rotate_dis;
    const auto new_wing_rot = new_wing_angle - wings_current_angle;
    left_wing_tran->local_z_rotate(-new_wing_rot);
    right_wing_tran->local_z_rotate(new_wing_rot);
    wings_current_angle = new_wing_angle;
}

void gearoenix::render::scene::Splash::hide()
{
    e.get_world()->get_component<scene::Scene>(scene_id)->set_enabled(false);
}
