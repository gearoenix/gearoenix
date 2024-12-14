#include "gx-cr-ecs-entity-builder.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "gx-cr-ecs-comp-type.hpp"
#include "gx-cr-ecs-world.hpp"
#include <imgui.h>

namespace {
const std::shared_ptr<gearoenix::core::ecs::Component> null_comp {};
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(const entity_id_t id, std::string&& name, job::EndCaller<>&& end_caller)
    : id(id)
    , name(std::move(name))
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(std::string&& name, job::EndCaller<>&& end_caller)
    : id(++Entity::last_id)
    , name(std::move(name))
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(EntityBuilder&& o) noexcept
    : id(o.id)
    , name(std::move(o.name))
    , final_types(o.final_types)
    , all_types(o.all_types)
    , all_types_to_components(std::move(o.all_types_to_components))
    , end_caller(std::move(o.end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::~EntityBuilder() = default;

void gearoenix::core::ecs::EntityBuilder::construct(
    const entity_id_t id,
    std::shared_ptr<platform::stream::Stream>&& stream,
    job::EndCallerShared<EntityBuilder>&& end_caller)
{
    job::send_job_to_pool([id, s = std::move(stream), e = std::move(end_caller)]() mutable {
        std::string name;
        s->read(name);
        e.set_return(std::shared_ptr<EntityBuilder>(new EntityBuilder(id, std::move(name), job::EndCaller([] { }))));
        const auto count = s->read<std::uint32_t>();
        for (auto i = decltype(count) { 0 }; i < count; ++i) {
            std::shared_ptr<platform::stream::Stream> ms = std::make_shared<platform::stream::Memory>();
            s->read(*ms);
            ms->seek(0);
            job::send_job_to_pool([ms = std::move(ms), e]() mutable {
                Component::read(std::move(ms), job::EndCallerShared<Component>([e = std::move(e)](std::shared_ptr<Component>&& c) {
                    e.get_return()->add_component(std::move(c));
                }));
            });
        }
    });
}

const std::shared_ptr<gearoenix::core::ecs::Component>& gearoenix::core::ecs::EntityBuilder::get_component(
    const component_index_t component_type) const
{
    const auto search = all_types_to_components.find(component_type);
    if (all_types_to_components.end() == search) {
        return null_comp;
    }
    return search->second;
}

void gearoenix::core::ecs::EntityBuilder::add_component(std::shared_ptr<Component>&& component)
{
    std::lock_guard _lg(components_lock);
    const auto fti = component->get_final_type_index();
    auto c = std::make_pair(fti, std::move(component));
    if (all_types.contains(fti)) {
        GX_LOG_F("Component '" << c.second->get_name() << "' already exists in entity '" << id);
    }
    for (const auto& pts = ComponentType::get_info(fti).get_all_parents(); const auto pt : pts) {
        if (all_types.contains(fti)) {
            GX_LOG_F("Component '" << c.second->get_name() << "' already exists in entity components hierarchy '" << id);
        }
        all_types.add(pt);
        all_types_to_components.emplace(pt, c.second);
    }
    all_types.add(fti);
    final_types.add(fti);
    all_types_to_components.emplace(std::move(c));
}

gearoenix::core::ecs::EntitySharedBuilder::EntitySharedBuilder(std::string&& name, job::EndCaller<>&& end_caller)
    : builder(std::move(name), std::move(end_caller))
{
}

gearoenix::core::ecs::EntitySharedBuilder::~EntitySharedBuilder()
{
    World::get()->delayed_create_entity(std::move(builder));
}
