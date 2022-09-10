#include "gx-rnd-scn-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-builder.hpp"
#include "../light/gx-rnd-lt-light.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../reflection/gx-rnd-rfl-baked.hpp"
#include "../reflection/gx-rnd-rfl-builder.hpp"
#include "../reflection/gx-rnd-rfl-runtime.hpp"
#include "../skybox/gx-rnd-sky-builder.hpp"
#include "../skybox/gx-rnd-sky-skybox.hpp"
#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Builder::Builder(engine::Engine& e, const std::string& name, const double layer) noexcept
    : entity_builder(e.get_world()->create_shared_builder())
{
    auto& b = entity_builder->get_builder();
    b.add_component(Scene(e, layer));
    b.set_name(name);
}

gearoenix::render::scene::Builder::~Builder() noexcept = default;

void gearoenix::render::scene::Builder::add(std::shared_ptr<model::Builder>&& model_builder) noexcept
{
    auto& b = entity_builder->get_builder();
    auto& mb = model_builder->get_entity_builder()->get_builder();
    auto* m = mb.get_component<model::Model>();
    GX_ASSERT_D(nullptr != m);
    b.get_component<Scene>()->add_model(mb.get_id(), *m);
    m->scene_id = b.get_id();
    const auto& name_opt = model_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(name_opt.has_value());
    std::string name = *name_opt;
    GX_ASSERT(!name.empty());
    GX_ASSERT(!model_builders.contains(name));
    model_builders.emplace(std::move(name), std::move(model_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<camera::Builder>&& camera_builder) noexcept
{
    auto& b = entity_builder->get_builder();
    auto& cb = camera_builder->get_entity_builder()->get_builder();
    auto* c = cb.get_component<camera::Camera>();
    GX_ASSERT_D(nullptr != c);
    b.get_component<Scene>()->add_camera(cb.get_id(), *c);
    c->set_scene_id(b.get_id());
    const auto& name_opt = camera_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(name_opt.has_value());
    std::string name = *name_opt;
    GX_ASSERT(!name.empty());
    GX_ASSERT(!camera_builders.contains(name));
    camera_builders.emplace(std::move(name), std::move(camera_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<reflection::Builder>&& reflection_builder) noexcept
{
    auto& b = entity_builder->get_builder();
    auto& rb = reflection_builder->get_entity_builder()->get_builder();
    if (auto* const rr = rb.get_component<reflection::Runtime>(); nullptr != rr) {
        b.get_component<Scene>()->add_runtime_reflection(rb.get_id(), *rr);
        rr->set_scene_id(b.get_id());
    }
    if (auto* const br = rb.get_component<reflection::Baked>(); nullptr != br) {
        b.get_component<Scene>()->add_baked_reflection(rb.get_id(), *br);
        br->set_scene_id(b.get_id());
    }
    for (const auto& camera_builder : reflection_builder->get_faces_camera_builders())
        if (nullptr != camera_builder)
            add(std::shared_ptr<camera::Builder>(camera_builder));
    const auto& name_opt = reflection_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(name_opt.has_value());
    std::string name = *name_opt;
    GX_ASSERT(!name.empty());
    GX_ASSERT(!reflection_builders.contains(name));
    reflection_builders.emplace(std::move(name), std::move(reflection_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<skybox::Builder>&& skybox_builder) noexcept
{
    auto& b = entity_builder->get_builder();
    auto& sb = skybox_builder->get_entity_builder()->get_builder();
    auto* s = sb.get_component<skybox::Skybox>();
    GX_ASSERT_D(nullptr != s);
    b.get_component<Scene>()->add_skybox(sb.get_id(), *s);
    s->set_scene_id(b.get_id());
    const auto& name_opt = skybox_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(name_opt.has_value());
    std::string name = *name_opt;
    GX_ASSERT(!name.empty());
    GX_ASSERT(!skybox_builders.contains(name));
    skybox_builders.emplace(std::move(name), std::move(skybox_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<light::Builder>&& light_builder) noexcept
{
    auto& b = entity_builder->get_builder();
    auto& lb = light_builder->get_entity_builder()->get_builder();
    auto* l = lb.get_component<light::Light>();
    GX_ASSERT_D(nullptr != l);
    b.get_component<Scene>()->add_light(lb.get_id(), *l);
    l->scene_id = b.get_id();
    for (const auto& camera_builder : light_builder->get_cameras())
        if (nullptr != camera_builder)
            add(std::shared_ptr<camera::Builder>(camera_builder));
    const auto& name_opt = light_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(name_opt.has_value());
    std::string name = *name_opt;
    GX_ASSERT(!name.empty());
    GX_ASSERT(!light_builders.contains(name));
    light_builders.emplace(std::move(name), std::move(light_builder));
}

gearoenix::render::scene::Scene& gearoenix::render::scene::Builder::get_scene() noexcept
{
    return *entity_builder->get_builder().get_component<Scene>();
}

const gearoenix::render::scene::Scene& gearoenix::render::scene::Builder::get_scene() const noexcept
{
    return *entity_builder->get_builder().get_component<Scene>();
}
