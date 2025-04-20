#include "gx-cr-object-streamer.hpp"
#include "../platform/stream/gx-plt-stm-memory.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "gx-cr-object.hpp"
#include "job/gx-cr-job-manager.hpp"

gearoenix::core::ObjectStreamer::ObjectStreamer(std::shared_ptr<platform::stream::Stream>&& s, const bool is_reading)
    : stream(std::move(s))
    , is_reading(is_reading)
{
}

void gearoenix::core::ObjectStreamer::initialise(std::optional<job::EndCaller<>>&& read_callback)
{
    auto shared_asset_map_offset = stream->tell();
    if (is_reading) {
        stream->read(shared_asset_map_offset);
        stream->seek(shared_asset_map_offset);
        GX_ASSERT_D(read_callback.has_value());
        const auto end = std::move(*read_callback);
        const auto shared_asset_map_size = stream->read<id_t>();
        for (id_t i = 0; i < shared_asset_map_size; ++i) {
            const auto id = stream->read<id_t>();
            std::shared_ptr<platform::stream::Stream> as = std::make_shared<platform::stream::Memory>();
            stream->read(*as);
            job::send_job_to_pool([this, self = self.lock(), id, as = std::move(as), end = end]() mutable -> void {
                job::EndCallerShared<Object> obj_end([this, self, id, end = std::move(end)](std::shared_ptr<Object>&& asset) mutable -> void {
                    const std::lock_guard _lg(lock);
                    auto* const info = &objects[id];
                    info->object = std::move(asset);
                    job::send_job_to_pool([info, end = std::move(end)] {
                        for (const auto& fun : info->read_functions) {
                            fun(info->object);
                        }
                    });
                });
                Object::read(std::move(as), std::move(self), std::move(obj_end));
            });
        }
        stream->seek(sizeof(shared_asset_map_offset));
    } else {
        GX_ASSERT_D(0 == shared_asset_map_offset);
        stream->write_fail_debug(shared_asset_map_offset);
    }
}

gearoenix::core::ObjectStreamer::~ObjectStreamer()
{
    if (is_reading) {
        return;
    }
    const auto shared_asset_map_offset = stream->tell();
    stream->write_fail_debug(static_cast<id_t>(objects.size()));
    for (const auto& [id, asset] : objects) {
        stream->write_fail_debug(id);
        stream->write(*asset.write_stream);
    }
    stream->seek(0);
    stream->write_fail_debug(shared_asset_map_offset);
}

std::shared_ptr<gearoenix::core::ObjectStreamer> gearoenix::core::ObjectStreamer::construct(
    std::shared_ptr<platform::stream::Stream>&& stream, std::optional<job::EndCaller<>>&& read_callback)
{
    auto result = std::shared_ptr<ObjectStreamer>(new ObjectStreamer(std::move(stream), read_callback.has_value()));
    result->self = result;
    result->initialise(std::move(read_callback));
    return result;
}

void gearoenix::core::ObjectStreamer::write(std::shared_ptr<Object>&& object)
{
    GX_ASSERT_D(!is_reading);
    const auto id = object->get_object_id();
    std::shared_ptr<platform::stream::Stream> write_stream;
    {
        const std::lock_guard _lg(lock);
        if (const auto search = objects.find(id); objects.end() != search) {
            return;
        }
        write_stream = std::make_shared<platform::stream::Memory>();
        objects.emplace(id, ObjectInfo { object, write_stream });
    }
    object->write(std::move(write_stream), self.lock());
}

void gearoenix::core::ObjectStreamer::read(const id_t id, std::function<void(const std::shared_ptr<Object>&)>&& fun)
{
    const std::shared_ptr<Object>* ptr = nullptr;
    {
        const std::lock_guard _lg(lock);
        const auto search = objects.find(id);
        if (objects.end() == search) {
            objects[id].read_functions.push_back(std::move(fun));
            return;
        }
        GX_ASSERT_D(nullptr != search->second.object);
        ptr = &search->second.object;
    }
    fun(*ptr);
}
