#include "gx-rnd-scn-splash.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../font/gx-rnd-fnt-font.hpp"
#include "../font/gx-rnd-fnt-manager.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
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
    auto& win_sz = platform::Application::get().get_base().get_window_size();
    const auto win_ar = static_cast<double>(win_sz.x) / static_cast<double>(win_sz.y);
    constexpr auto threshold = 1.5;
    constexpr auto out = 1.3;
    if (win_ar > threshold) {
        return out;
    }
    return win_ar * (out / threshold);
}

gearoenix::render::scene::Splash::Splash(core::job::EndCaller<>&& end_callback)
    : wings_current_angle(wing_angle_start)
    , current_scale(calculate_scale() * whole_scale_start)
    , end_callback(std::move(end_callback))
{
}

void gearoenix::render::scene::Splash::initialise(core::job::EndCaller<>&& start_callback)
{
    std::vector<PbrVertex> plane_vertices(4);
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

    auto entity_callback = core::job::EndCaller([this, e = start_callback]() -> void {
        scene_entity->get_component<Scene>()->set_enabled(true);
        (void)e;
    });

    scene_entity = Manager::get().build("gearoenix-splash-scene", 0.0);

    camera::Manager::get().build(
        "gearoenix-splash-camera", scene_entity.get(),
        core::job::EndCaller<core::ecs::EntityPtr>([](auto&& ce) {
            ce->template get_component<physics::Transformation>()->set_local_position({ 0.0, 0.0, 5.0 });
            auto& camera = *ce->template get_component<camera::Camera>();
            camera.set_projection_data(camera::ProjectionData::construct_orthographic(1.0f));
            camera.disable_bloom();
            camera.get_colour_tuning() = camera::ColourTuning::Unchanged {};
        }));

    struct Values {
        core::job::EndCaller<> entity_callback;
        std::shared_ptr<Splash> splash;

        std::shared_ptr<mesh::Mesh> bg_mesh;
        std::shared_ptr<mesh::Mesh> gear_mesh;
        std::shared_ptr<mesh::Mesh> glare_mesh;
        std::shared_ptr<mesh::Mesh> text_mesh;
        std::shared_ptr<mesh::Mesh> left_wing_mesh;
        std::shared_ptr<mesh::Mesh> right_wing_mesh;

        double text_width = 0.0, text_height = 0.0;

        Values(std::shared_ptr<Splash>&& splash, core::job::EndCaller<>&& entity_callback)
            : entity_callback(std::move(entity_callback))
            , splash(std::move(splash))
        {
        }

        ~Values()
        {
            {
                auto entity = model::Manager::get().build(
                    "gearoenix-splash-bg", splash->scene_entity.get(),
                    { std::move(bg_mesh) }, true);
                entity->get_component<physics::Transformation>()->local_inner_scale(100.0);
            }

            {
                auto entity = model::Manager::get().build(
                    "gearoenix-splash-gear", splash->scene_entity.get(),
                    { std::shared_ptr(gear_mesh) }, true);
                auto* const trn = entity->get_component<physics::Transformation>();
                trn->local_inner_scale(splash->current_scale * gear_base_scale);
                trn->local_translate({ 0.0f, 0.0f, 0.3f });
                splash->gear_tran = trn;
            }

            {
                auto entity = model::Manager::get().build(
                    "gearoenix-splash-glare", splash->scene_entity.get(),
                    { std::move(glare_mesh) }, true);
                auto* const trn = entity->get_component<physics::Transformation>();
                trn->local_inner_scale(splash->current_scale * glare_base_scale);
                trn->local_translate({ 0.0f, 0.0f, 0.2f });
                splash->glare_tran = trn;
            }

            {
                auto entity = model::Manager::get().build(
                    "gearoenix-splash-left-wing", splash->scene_entity.get(),
                    { std::move(left_wing_mesh) }, true);
                auto* const trn = entity->get_component<physics::Transformation>();
                trn->local_inner_scale(splash->current_scale * wings_base_scale);
                trn->local_translate({ 0.0, 0.0, 0.1 });
                trn->local_inner_z_rotate(-wing_angle_start);
                splash->left_wing_tran = trn;
            }

            {
                auto entity = model::Manager::get().build(
                    "gearoenix-splash-right-wing", splash->scene_entity.get(),
                    { std::move(right_wing_mesh) }, true);
                auto* const trn = entity->get_component<physics::Transformation>();
                trn->local_inner_scale(splash->current_scale * wings_base_scale);
                trn->local_translate({ 0.0f, 0.0f, 0.1f });
                trn->local_inner_z_rotate(wing_angle_start);
                splash->right_wing_tran = trn;
            }

            {
                math::Vec3 scale(splash->current_scale * gearoenix_text_base_scale);
                scale.x *= text_width / text_height;
                auto entity = model::Manager::get().build(
                    "gearoenix-splash-text", splash->scene_entity.get(),
                    { std::move(text_mesh) }, true);
                auto* const trn = entity->get_component<physics::Transformation>();
                trn->local_inner_scale(scale);
                trn->local_translate({ 0.0f, -0.25f, 0.5f });
                splash->gearoenix_text_tran = trn;
                splash->gearoenix_text_mat = dynamic_cast<material::Unlit*>(entity->get_component<model::Model>()->get_meshes().begin()->get()->get_bound_material().get());
            }
        }
    };

    auto values = std::make_shared<Values>(weak_self.lock(), std::move(entity_callback));

    auto font = font::Manager::get().get_font("Roboto-Bold");

    material::Manager::get().get_unlit(
        "gearoenix-splash-bg",
        core::job::EndCallerShared<material::Unlit>([values](std::shared_ptr<material::Unlit>&& m) {
            mesh::Manager::get().build_plate(
                std::move(m),
                core::job::EndCallerShared<mesh::Mesh>([values](std::shared_ptr<mesh::Mesh>&& m) {
                    values->bg_mesh = std::move(m);
                }));
        }));

    texture::Manager::get().create_2d_from_file(
        platform::stream::Path::create_asset("gearoenix-splash/gear.png"),
        texture::TextureInfo {},
        core::job::EndCallerShared<texture::Texture2D>([values](std::shared_ptr<texture::Texture2D>&& t) {
            material::Manager::get().get_unlit(
                "gearoenix-splash-gear",
                core::job::EndCallerShared<material::Unlit>(
                    [values, t = std::move(t)](std::shared_ptr<material::Unlit>&& m) mutable {
                        m->set_albedo(std::move(t));
                        m->set_transparency(material::Transparency::Transparent);
                        mesh::Manager::get().build_plate(
                            std::move(m),
                            core::job::EndCallerShared<mesh::Mesh>(
                                [values](std::shared_ptr<mesh::Mesh>&& m) {
                                    values->gear_mesh = std::move(m);
                                }));
                    }));
        }));

    texture::Manager::get().create_2d_from_file(
        platform::stream::Path::create_asset("gearoenix-splash/glare.png"),
        texture::TextureInfo(),
        core::job::EndCallerShared<texture::Texture2D>([values](std::shared_ptr<texture::Texture2D>&& t) {
            material::Manager::get().get_unlit(
                "gearoenix-splash-glare",
                core::job::EndCallerShared<material::Unlit>(
                    [values, t = std::move(t)](std::shared_ptr<material::Unlit>&& m) mutable {
                        m->set_albedo(std::move(t));
                        m->set_transparency(material::Transparency::Transparent);
                        mesh::Manager::get().build_plate(
                            std::move(m),
                            core::job::EndCallerShared<mesh::Mesh>(
                                [values](std::shared_ptr<mesh::Mesh>&& m) {
                                    values->glare_mesh = std::move(m);
                                }));
                    }));
        }));

    material::Manager::get().get_unlit(
        "gearoenix-splash-wing",
        core::job::EndCallerShared<material::Unlit>(
            [values, lv = std::move(left_wing_vertices), rv = std::move(right_wing_vertices), ib = std::move(plane_indices)](std::shared_ptr<material::Unlit>&& m) mutable {
                m->set_transparency(material::Transparency::Transparent);
                texture::Manager::get().create_2d_from_file(
                    gearoenix::platform::stream::Path::create_asset("gearoenix-splash/wing.png"),
                    gearoenix::render::texture::TextureInfo(),
                    core::job::EndCallerShared<texture::Texture2D>(
                        [values, mat = std::move(m), lv = std::move(lv), rv = std::move(rv), ib = std::move(ib)](
                            std::shared_ptr<texture::Texture2D>&& t) mutable {
                            mat->set_albedo(std::move(t));
                            mesh::Manager::get().build(
                                "gearoenix-splash-left-wing",
                                std::move(lv),
                                std::vector(ib),
                                std::shared_ptr(mat),
                                core::job::EndCallerShared<mesh::Mesh>(
                                    [values](std::shared_ptr<mesh::Mesh>&& m) {
                                        values->left_wing_mesh = std::move(m);
                                    }));

                            mesh::Manager::get().build(
                                "gearoenix-splash-right-wing",
                                std::move(rv),
                                std::move(ib),
                                std::move(mat),
                                core::job::EndCallerShared<mesh::Mesh>(
                                    [values](std::shared_ptr<mesh::Mesh>&& m) {
                                        values->right_wing_mesh = std::move(m);
                                    }));
                        }));
            }));

    values->text_height = platform::Application::get().get_base().get_screen_size().y * math::Numeric::epsilon<double>;

    material::Manager::get().get_unlit(
        "gearoenix-splash-text",
        core::job::EndCallerShared<material::Unlit>([values, font = std::move(font)](std::shared_ptr<material::Unlit>&& m) mutable {
            m->set_transparency(material::Transparency::Transparent);
            m->get_albedo_factor() = { 0.4f, 0.0f, 0.0f, 0.0f };
            font->bake(
                L"Gearoenix", math::Vec4(1.0), values->text_height, values->text_width,
                core::job::EndCallerShared<texture::Texture2D>(
                    [values, m = std::move(m)](std::shared_ptr<texture::Texture2D>&& t) mutable {
                        m->set_albedo(std::move(t));
                        mesh::Manager::get().build_plate(
                            std::move(m),
                            core::job::EndCallerShared<mesh::Mesh>(
                                [values](std::shared_ptr<mesh::Mesh>&& m) {
                                    values->text_mesh = std::move(m);
                                }));
                    }));
        }));
}

std::shared_ptr<gearoenix::render::scene::Splash> gearoenix::render::scene::Splash::construct(
    core::job::EndCaller<>&& start_callback,
    core::job::EndCaller<>&& end_callback)
{
    std::shared_ptr<Splash> result(new Splash(std::move(end_callback)));
    result->weak_self = result;
    result->initialise(std::move(start_callback));
    return result;
}

void gearoenix::render::scene::Splash::update()
{
    if (gear_tran == nullptr || glare_tran == nullptr || left_wing_tran == nullptr || right_wing_tran == nullptr || gearoenix_text_tran == nullptr || gearoenix_text_mat == nullptr) {
        return;
    }

    const auto delta_time = math::Numeric::safe_minimum(engine::Engine::get().get_delta_time(), 0.05);
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
        gearoenix_text_mat->get_albedo_factor().w = static_cast<float>(text_animation_time_current);
    } else {
        after_animation += delta_time;
        if (after_animation > 1.0) {
            end_callback = std::nullopt;
            (void)end_callback;
        }
    }

    gear_tran->local_inner_z_rotate(delta_time);
    glare_tran->local_inner_z_rotate(-delta_time);

    const auto new_scale = calculate_scale() * (whole_scale_start + whole_scale_animation * scale_animation_time_current);
    const auto scale = new_scale / current_scale;
    current_scale = new_scale;

    gear_tran->local_inner_scale(scale);
    glare_tran->local_inner_scale(scale);
    left_wing_tran->local_inner_scale(scale);
    right_wing_tran->local_inner_scale(scale);
    gearoenix_text_tran->local_inner_scale(scale);

    const auto new_wing_angle = wing_angle_start + scale_animation_time_current * wing_rotate_dis;
    const auto new_wing_rot = new_wing_angle - wings_current_angle;
    left_wing_tran->local_inner_z_rotate(-new_wing_rot);
    right_wing_tran->local_inner_z_rotate(new_wing_rot);
    wings_current_angle = new_wing_angle;
}

void gearoenix::render::scene::Splash::hide()
{
    scene_entity->get_component<Scene>()->set_enabled(false);
}
