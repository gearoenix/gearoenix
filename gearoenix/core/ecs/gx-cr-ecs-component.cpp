#include "gx-cr-ecs-component.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-table.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include "../gx-cr-object-streamer.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../macro/gx-cr-mcr-stringifier.hpp"
#include "gx-cr-ecs-comp-type.hpp"
#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Component::Component(Entity* const entity, const object_type_index_t final_type_index, std::string&& name)
    : Object(final_type_index, std::move(name))
    , entity(entity)
{
    GX_ASSERT_D(ComponentType::check(final_type_index));
}

gearoenix::core::ecs::Component::Component(Entity* const entity, const object_type_index_t final_type_index, const object_id_t object_id, std::string&& name)
    : Object(final_type_index, object_id, std::move(name))
    , entity(entity)
{
    GX_ASSERT_D(ComponentType::check(final_type_index));
}

void gearoenix::core::ecs::Component::write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& object_streamer, job::EndCaller<>&&)
{
    stream->write_fail_debug(enabled);
    GX_ASSERT_D(entity);
    stream->write_fail_debug(entity->get_object_id());
    object_streamer->write(entity->get_object_self().lock());
}

void gearoenix::core::ecs::Component::read(std::shared_ptr<Component>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& object_streamer, job::EndCaller<>&&)
{
    self->enabled = stream->read<bool>();
    const auto entity_id = stream->read<object_id_t>();
    // Don't capture the end here because, if the call is from an entity, you will enter a loop of dependency
    // The entity depends on the component to end and the component depends on the entity to end.
    object_streamer->read(entity_id, [self = std::move(self)](const std::shared_ptr<Object>& e) {
        self->set_entity(cast_ptr<Entity>(e.get()));
    });
}

gearoenix::core::ecs::Component::~Component() = default;

void gearoenix::core::ecs::Component::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        Object::show_debug_gui();

        render::imgui::table_scope("##gearoenix::core::ecs::Component", [this] {
            ImGui::Text("Enabled:");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##enabled", &enabled);
            ImGui::TableNextColumn();

            ImGui::Text("Entity ID:");
            ImGui::TableNextColumn();
            ImGui::Text("%u", entity->get_object_id());
        });

        if (nullptr != entity && ImGui::TreeNode("Entity")) {
            entity->show_debug_gui();
            ImGui::TreePop();
        }
    });
}

void gearoenix::core::ecs::Component::set_entity(Entity* const e)
{
    entity = e;
}
