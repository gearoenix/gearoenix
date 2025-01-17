#include "gx-rnd-scn-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
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

gearoenix::render::scene::Builder::Builder(
    engine::Engine& e, const std::string& name, const double layer, core::job::EndCaller<>&& end_callback)
    : e(e)
    , entity_builder(std::make_shared<core::ecs::EntitySharedBuilder>(std::string(name), std::move(end_callback)))
{
    auto& b = entity_builder->get_builder();
    b.add_component(core::ecs::construct_component<Scene>(e, layer, name + "-scene", b.get_id()));
}

gearoenix::render::scene::Builder::~Builder() = default;

void gearoenix::render::scene::Builder::add(std::shared_ptr<model::Builder>&& model_builder)
{
    auto& b = entity_builder->get_builder();
    auto& mb = model_builder->get_entity_builder()->get_builder();
    auto* m = mb.get_component<model::Model>();
    GX_ASSERT_D(nullptr != m);
    b.get_component<Scene>()->add_model(mb.get_id(), *m);
    m->scene_id = b.get_id();
    const auto& name = model_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(!name.empty());
    GX_ASSERT(!model_builders.contains(name));
    model_builders.emplace(std::string(name), std::move(model_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<camera::Builder>&& camera_builder)
{
    auto& b = entity_builder->get_builder();
    auto& cb = camera_builder->get_entity_builder()->get_builder();
    auto* c = cb.get_component<camera::Camera>();
    GX_ASSERT_D(nullptr != c);
    b.get_component<Scene>()->add_camera(cb.get_id(), *c);
    c->set_scene_id(b.get_id());
    const auto& name = camera_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(!name.empty());
    GX_ASSERT(!camera_builders.contains(name));
    camera_builders.emplace(std::string(name), std::move(camera_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<reflection::Builder>&& reflection_builder)
{
    const auto& b = entity_builder->get_builder();
    const auto& rb = reflection_builder->get_entity_builder()->get_builder();
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
    const auto& name = reflection_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(!name.empty());
    GX_ASSERT(!reflection_builders.contains(name));
    reflection_builders.emplace(std::string(name), std::move(reflection_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<skybox::Builder>&& skybox_builder)
{
    auto& b = entity_builder->get_builder();
    auto& sb = skybox_builder->get_entity_builder()->get_builder();
    auto* s = sb.get_component<skybox::Skybox>();
    GX_ASSERT_D(nullptr != s);
    b.get_component<Scene>()->add_skybox(sb.get_id(), *s);
    s->set_scene_id(b.get_id());
    const auto& name = skybox_builder->get_entity_builder()->get_builder().get_name();
    GX_ASSERT(!name.empty());
    GX_ASSERT(!skybox_builders.contains(name));
    skybox_builders.emplace(std::string(name), std::move(skybox_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<light::Builder>&& light_builder)
{
    auto& b = entity_builder->get_builder();
    auto& lb = light_builder->get_entity_builder()->get_builder();
    auto* l = lb.get_component<light::Light>();
    GX_ASSERT_D(nullptr != l);
    b.get_component<Scene>()->add_light(lb.get_id(), *l);
    l->scene_id = b.get_id();
    for (const auto& camera_builder : light_builder->camera_builders) {
        if (nullptr != camera_builder) {
            add(std::shared_ptr(camera_builder));
        }
    }
    const auto& name = light_builder->get_entity_builder()->get_name();
    GX_ASSERT(!name.empty());
    GX_ASSERT(!light_builders.contains(name));
    light_builders.emplace(std::string(name), std::move(light_builder));
}

void gearoenix::render::scene::Builder::add_armature(std::shared_ptr<core::ecs::EntitySharedBuilder>&& armature_builder)
{
    auto& b = entity_builder->get_builder();
    auto& ab = armature_builder->get_builder();
    auto* a = ab.get_component<physics::animation::Armature>();
    GX_ASSERT_D(nullptr != a);
    b.get_component<Scene>()->add_armature(ab.get_id());
    a->set_scene_id(b.get_id());
    const auto& name = armature_builder->get_name();
    GX_ASSERT(!name.empty());
    GX_ASSERT(!armature_builders.contains(name));
    armature_builders.emplace(std::string(name), std::move(armature_builder));
}

gearoenix::render::scene::Scene& gearoenix::render::scene::Builder::get_scene()
{
    return *entity_builder->get_builder().get_component<Scene>();
}

const gearoenix::render::scene::Scene& gearoenix::render::scene::Builder::get_scene() const
{
    return *entity_builder->get_builder().get_component<Scene>();
}

gearoenix::core::ecs::entity_id_t gearoenix::render::scene::Builder::get_id() const
{
    return entity_builder->get_id();
}
