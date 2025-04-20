#include "gx-rnd-mdl-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../mesh/gx-rnd-msh-buffer.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "gx-rnd-mdl-model.hpp"

namespace {
gearoenix::render::model::Manager* instance = nullptr;
}

gearoenix::render::model::Manager::Manager()
{
    GX_ASSERT_D(!instance);
    instance = this;
    core::ecs::ComponentType::add<Model>();
}

gearoenix::render::model::Manager::~Manager()
{
    GX_ASSERT_D(instance);
    instance = nullptr;
}

gearoenix::render::model::Manager& gearoenix::render::model::Manager::get()
{
    GX_ASSERT_D(instance);
    return *instance;
}

gearoenix::core::ecs::EntityPtr gearoenix::render::model::Manager::build(std::string&& name, core::ecs::Entity* const parent, meshes_set_t&& meshes, const bool)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    math::Aabb3<double> box;
    for (const auto& m : meshes) {
        box.put(m->get_buffer()->get_box());
    }
    auto transform = core::Object::construct<physics::Transformation>(entity->get_object_name() + "-transformation");
    entity->add_component(core::Object::construct<physics::collider::Aabb3>(std::shared_ptr(transform), box, entity->get_object_name() + "-collider"));
    entity->add_component(std::move(transform));
    return entity;
}
