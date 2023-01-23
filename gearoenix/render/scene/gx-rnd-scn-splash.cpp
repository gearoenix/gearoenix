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

double gearoenix::render::scene::Splash::calculate_scale() const noexcept
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
    const core::sync::EndCaller& start_callback,
    const core::sync::EndCaller& end_callback) noexcept
    : e(e)
    , end_callback(end_callback)
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

    auto scene_callback = gearoenix::core::sync::EndCaller([this, end = start_callback]() noexcept -> void {
        this->e.get_world()->get_component<Scene>(scene_id)->enabled = true;
    });

    const auto scene_builder = e.get_scene_manager()->build(
        "gearoenix-splash-scene", 0.0, core::sync::EndCaller(scene_callback));
    scene_id = scene_builder->get_entity_builder()->get_builder().get_id();

    auto plane_mesh = e.get_mesh_manager()->build(
        "gearoenix-splash-plane",
        std::move(plane_vertices),
        std::move(plane_indices),
        scene_callback);
    auto left_wing_mesh = e.get_mesh_manager()->build(
        "gearoenix-splash-left-wing",
        std::move(left_wing_vertices),
        std::move(left_wing_indices),
        scene_callback);
    auto right_wing_mesh = e.get_mesh_manager()->build(
        "gearoenix-splash-right-wing",
        std::move(right_wing_vertices),
        std::move(right_wing_indices),
        scene_callback);
    {
        auto bg_mat = e.get_material_manager()->get_unlit("gearoenix-splash-bg", scene_callback);
        auto bg_model_builder = e.get_model_manager()->build(
            "gearoenix-splash-bg",
            std::shared_ptr(plane_mesh),
            std::move(bg_mat),
            core::sync::EndCaller(scene_callback),
            true);
        bg_model_builder->get_transformation().local_scale(100.0);
        scene_builder->add(std::move(bg_model_builder));
    }
    current_scale = calculate_scale() * whole_scale_start;
    {
        auto gear_mat = e.get_material_manager()->get_unlit("gearoenix-splash-gear", scene_callback);
        gear_mat->set_transparency(material::Transparency::Transparent);
        gear_mat->set_albedo(e.get_texture_manager()->create_2d_from_file(
            "gearoenix-splash-gear",
            gearoenix::platform::stream::Path::create_asset("gearoenix-splash/gear.png"),
            gearoenix::render::texture::TextureInfo(),
            scene_callback));
        auto gear_model_builder = e.get_model_manager()->build(
            "gearoenix-splash-gear",
            std::shared_ptr(plane_mesh),
            std::move(gear_mat),
            core::sync::EndCaller(scene_callback),
            true);
        gear_model_builder->get_transformation().local_scale(current_scale * gear_base_scale);
        gear_model_builder->get_transformation().local_translate({ 0.0f, 0.0f, 0.3f });
        gear_id = gear_model_builder->get_entity_builder()->get_builder().get_id();
        scene_builder->add(std::move(gear_model_builder));
    }

    {
        auto glare_mat = e.get_material_manager()->get_unlit("gearoenix-splash-glare", scene_callback);
        glare_mat->set_transparency(material::Transparency::Transparent);
        glare_mat->set_albedo(e.get_texture_manager()->create_2d_from_file(
            "gearoenix-splash-glare",
            gearoenix::platform::stream::Path::create_asset("gearoenix-splash/glare.png"),
            gearoenix::render::texture::TextureInfo(),
            scene_callback));
        auto glare_model_builder = e.get_model_manager()->build(
            "gearoenix-splash-glare",
            std::shared_ptr(plane_mesh),
            std::move(glare_mat),
            core::sync::EndCaller(scene_callback),
            true);
        glare_model_builder->get_transformation().local_scale(current_scale * glare_base_scale);
        glare_model_builder->get_transformation().local_translate({ 0.0f, 0.0f, 0.2f });
        glare_id = glare_model_builder->get_entity_builder()->get_builder().get_id();
        scene_builder->add(std::move(glare_model_builder));
    }

    auto wing_mat = e.get_material_manager()->get_unlit("gearoenix-splash-wing", scene_callback);
    wing_mat->set_transparency(material::Transparency::Transparent);
    wing_mat->set_albedo(e.get_texture_manager()->create_2d_from_file(
        "gearoenix-splash-wing",
        gearoenix::platform::stream::Path::create_asset("gearoenix-splash/wing.png"),
        gearoenix::render::texture::TextureInfo(),
        scene_callback));

    wings_current_angle = wing_angle_start;
    {
        auto wing_model_builder = e.get_model_manager()->build(
            "gearoenix-splash-left-wing",
            std::move(left_wing_mesh),
            std::shared_ptr(wing_mat),
            core::sync::EndCaller(scene_callback),
            true);
        wing_model_builder->get_transformation().local_scale(current_scale * wings_base_scale);
        wing_model_builder->get_transformation().local_translate({ 0.0, 0.0, 0.1 });
        wing_model_builder->get_transformation().local_z_rotate(-wing_angle_start);
        left_wing_id = wing_model_builder->get_entity_builder()->get_builder().get_id();
        scene_builder->add(std::move(wing_model_builder));
    }

    {
        auto wing_model_builder = e.get_model_manager()->build(
            "gearoenix-splash-right-wing",
            std::move(right_wing_mesh),
            std::move(wing_mat),
            core::sync::EndCaller(scene_callback),
            true);
        wing_model_builder->get_transformation().local_scale(current_scale * wings_base_scale);
        wing_model_builder->get_transformation().local_translate({ 0.0f, 0.0f, 0.1f });
        wing_model_builder->get_transformation().local_z_rotate(wing_angle_start);
        right_wing_id = wing_model_builder->get_entity_builder()->get_builder().get_id();
        scene_builder->add(std::move(wing_model_builder));
    }

    {
        auto text_mat = e.get_material_manager()->get_unlit("gearoenix-splash-text", scene_callback);
        text_mat->set_transparency(material::Transparency::Transparent);
        text_mat->get_albedo_factor() = { 0.4f, 0.0f, 0.0f, 0.0f };
        const auto font = e.get_font_manager()->get("Roboto-Bold");
        const auto text_height = e.get_platform_application().get_base().get_screen_size().y * 0.0001;
        double text_width = 0.0;
        const auto text_texture = font->bake(
            L"Gearoenix", math::Vec4(1.0), text_height, text_width, scene_callback);
        math::Vec3<double> scale(current_scale * gearoenix_text_base_scale);
        scale.x *= text_width / text_height;
        text_mat->set_albedo(text_texture);
        auto model_builder = e.get_model_manager()->build(
            "gearoenix-splash-text",
            std::shared_ptr(plane_mesh),
            std::move(text_mat),
            core::sync::EndCaller(scene_callback),
            true);
        model_builder->get_transformation().local_scale(scale);
        model_builder->get_transformation().local_translate({ 0.0f, -0.25f, 0.5f });
        gearoenix_text = model_builder->get_entity_builder()->get_builder().get_id();
        scene_builder->add(std::move(model_builder));
    }

    auto camera_builder = e.get_camera_manager()->build(
        "gearoenix-splash-camera", core::sync::EndCaller(scene_callback));
    camera_builder->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });
    auto& camera = camera_builder->get_camera();
    camera.set_projection_type(camera::Projection::Orthographic);
    camera_builder->disable_bloom();
    camera.get_colour_tuning() = camera::RawColour {};
    scene_builder->add(std::move(camera_builder));
}

void gearoenix::render::scene::Splash::update() noexcept
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

void gearoenix::render::scene::Splash::hide() noexcept
{
    e.get_world()->get_component<scene::Scene>(scene_id)->enabled = false;
}
