#include "gx-cr-object.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "../render/imgui/gx-rnd-imgui-type-table.hpp"
#include "../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include "ecs/gx-cr-ecs-entity.hpp"

gearoenix::core::Object::ObjectTypeInfos gearoenix::core::Object::object_type_infos;
boost::container::flat_map<std::string, gearoenix::core::object_type_index_t> gearoenix::core::Object::object_type_name_to_index;
std::atomic<gearoenix::core::object_id_t> gearoenix::core::Object::latest_id = 8;
std::mutex gearoenix::core::Object::all_objects_lock;
std::vector<std::weak_ptr<gearoenix::core::Object>> gearoenix::core::Object::all_objects = {};

void gearoenix::core::Object::register_type(const object_type_index_t t, ObjectTypeInfo&& info)
{
    while (static_cast<object_type_index_t>(object_type_infos.size()) <= t) {
        object_type_infos.emplace_back();
    }

    auto& ti = object_type_infos[t];

    if (ti.get_name() == info.get_name()) {
        return;
    }
    GX_ASSERT_D(!is_registered(t));
    GX_ASSERT_D(object_type_name_to_index.find(info.get_name()) == object_type_name_to_index.end());

    object_type_name_to_index.emplace(info.get_name(), t);
    ti = std::move(info);

    GX_ASSERT_D(check_object_type_registration_state(t));
}

gearoenix::core::Object::Object(const object_type_index_t final_type_index, std::string&& name)
    : object_final_type_index(final_type_index)
    , object_id(++latest_id)
    , object_name(std::move(name))
{
    GX_ASSERT_D(check_object_type_registration_state(final_type_index));
}

gearoenix::core::Object::Object(const object_type_index_t final_type_index, const object_id_t id, std::string&& name)
    : object_final_type_index(final_type_index)
    , object_id(id)
    , object_name(std::move(name))
{
    GX_ASSERT_D(check_object_type_registration_state(final_type_index));
}

void gearoenix::core::Object::write(std::shared_ptr<platform::stream::Stream>&&, std::shared_ptr<ObjectStreamer>&&, job::EndCaller<>&&)
{
    GX_UNEXPECTED; // your type must implement this function.
}

gearoenix::core::Object::~Object() = default;

void gearoenix::core::Object::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        render::imgui::table_scope("##gearoenix::core::Object", [this] {
            ImGui::Text("Name:");
            ImGui::TableNextColumn();
            ImGui::Text("%s", object_name.c_str());
            ImGui::TableNextColumn();

            ImGui::Text("Final Type: ");
            ImGui::TableNextColumn();
            ImGui::Text("%s", get_object_type_name(object_final_type_index).c_str());
            ImGui::TableNextColumn();

            ImGui::Text("Object ID:");
            ImGui::TableNextColumn();
            ImGui::Text("%u", object_id);
        });
    });
}

void gearoenix::core::Object::register_object(const std::shared_ptr<Object>& object)
{
    object->object_self = object;

    std::lock_guard _l(all_objects_lock);
    while (all_objects.size() <= object->object_id) {
        all_objects.emplace_back();
    }
    all_objects[object->object_id] = object;
}

const gearoenix::core::Object::ObjectTypeInfo& gearoenix::core::Object::get_type_info(const object_type_index_t ti)
{
    GX_ASSERT_D(ti < static_cast<object_type_index_t>(object_type_infos.size()));
    return object_type_infos[ti];
}

bool gearoenix::core::Object::is_registered(const object_type_index_t ti) { return ti < static_cast<object_type_index_t>(object_type_infos.size()) && !object_type_infos[ti].get_name().empty(); }

std::optional<gearoenix::core::object_type_index_t> gearoenix::core::Object::get_index(const std::string& type_name)
{
    const auto search = object_type_name_to_index.find(type_name);
    return search == object_type_name_to_index.end() ? std::nullopt : std::make_optional(search->second);
}

const std::string& gearoenix::core::Object::get_object_type_name(const object_type_index_t ti)
{
    GX_ASSERT_D(ti < static_cast<object_type_index_t>(object_type_infos.size()));
    return object_type_infos[ti].get_name();
}

const gearoenix::core::Object::ObjectStreamConstructor& gearoenix::core::Object::get_object_stream_constructor(const object_type_index_t ti)
{
    GX_ASSERT_D(ti < static_cast<object_type_index_t>(object_type_infos.size()));
    return object_type_infos[ti].get_construct_stream();
}

bool gearoenix::core::Object::check_object_type_registration_state(const object_type_index_t ti)
{
    const auto check_fn = [](const object_type_index_t t) {
        if (!is_registered(t)) {
            return true;
        }
        {
            const auto& name = get_object_type_name(t);
            if (const auto search = object_type_name_to_index.find(name); search == object_type_name_to_index.end() || search->second != t) {
                return true;
            }
        }
        return false;
    };
    if (check_fn(ti)) {
        return false;
    }
    for (const auto& parents = object_type_infos[ti].get_all_parents(); const auto pt : parents) {
        if (check_fn(pt)) {
            return false;
        }
    }
    return true;
}

const gearoenix::core::ObjectTypeIndexSet& gearoenix::core::Object::get_all_parent_types() const { return get_type_info(object_final_type_index).get_all_parents(); }

std::shared_ptr<gearoenix::core::Object> gearoenix::core::Object::find_object(const object_id_t id)
{
    if (id < all_objects.size()) {
        return all_objects[id].lock();
    }
    return nullptr;
}

bool gearoenix::core::Object::is_castable_to(const object_type_index_t ti) const
{
    if (object_final_type_index == ti) {
        return true;
    }
    return get_all_parent_types().contains(ti);
}

void gearoenix::core::Object::read(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& stream_context, job::EndCallerShared<Object>&& end_callback)
{
    const auto fti = stream->read<object_type_index_t>();
    const auto id = stream->read<object_id_t>();
    std::string name;
    stream->read(name);
    const auto& info = get_type_info(fti);
    const auto& constructor = info.get_construct_stream();
    constructor(id, std::move(name), std::move(stream), std::move(stream_context), std::move(end_callback));
}

void gearoenix::core::Object::write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& object_streamer)
{
    stream->write_fail_debug(object_final_type_index);
    stream->write_fail_debug(object_id);
    stream->write_fail_debug(object_name);
    write(std::move(stream), std::move(object_streamer), job::EndCaller([] { }));
}
