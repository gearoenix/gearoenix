#include "gx-cr-object-streamer.hpp"
#include "../platform/stream/gx-plt-stm-memory.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "gx-cr-object.hpp"
#include "job/gx-cr-job-manager.hpp"

#include <ranges>

gearoenix::core::ObjectStreamer::ObjectStreamer(std::shared_ptr<platform::stream::Stream>&& s, const bool is_reading)
    : stream(std::move(s))
    , is_reading(is_reading)
{
    // This stream must start from zero, and it should only belong to this object.
    GX_ASSERT_D(0 == stream->tell());
}

void gearoenix::core::ObjectStreamer::read()
{
    // This stream must start from zero, and it should only belong to this object.
    GX_ASSERT_D(0 == stream->tell());
    GX_ASSERT_D(is_reading);
    const auto shared_asset_map_size = stream->read<id_t>();
    std::vector<id_t> ids(shared_asset_map_size);
    for (id_t i = 0; i < shared_asset_map_size; ++i) {
        const auto id = stream->read<id_t>();
        objects[id] = {};
        ids[i] = id;
    }
    for (id_t i = 0; i < shared_asset_map_size; ++i) {
        const auto id = ids[i];
        std::shared_ptr<platform::stream::Stream> as = std::make_shared<platform::stream::Memory>();
        stream->read(*as);
        job::send_job_to_pool([this, self = weak_self.lock(), id, as = std::move(as)]() mutable -> void {
            job::EndCallerShared<Object> obj_end([this, self, id](std::shared_ptr<Object>&& asset) mutable -> void {
                GX_ASSERT_D(asset);
                const std::lock_guard _lg(lock);
                GX_ASSERT_D(objects.contains(id));
                auto* const info = &objects[id];
                GX_ASSERT_D(nullptr == info->object);
                info->object = std::move(asset);
                for (const auto& fun : info->read_functions) {
                    job::send_job_to_pool([fun = fun, self, info] {
                        fun(info->object); // Maybe in future `end` needs to be passed here.
                        (void)self;
                    });
                }
                info->read_functions.clear();
            });
            Object::read(std::move(as), std::move(self), std::move(obj_end));
        });
    }
}

gearoenix::core::ObjectStreamer::~ObjectStreamer()
{
    if (is_reading) {
        GX_ASSERT_D(!write_callback.has_value());
        return;
    }
    GX_ASSERT_D(write_callback.has_value());
    // This stream must start from zero, and it should only belong to this object.
    // No one else should write to this stream.
    GX_ASSERT_D(0 == stream->tell());
    stream->write_fail_debug(static_cast<id_t>(objects.size()));
    for (const auto& id : objects | std::views::keys) {
        stream->write_fail_debug(id);
    }
    for (const auto& asset : objects | std::views::values) {
        stream->write(*asset.write_stream);
    }
}

std::shared_ptr<gearoenix::core::ObjectStreamer> gearoenix::core::ObjectStreamer::construct_writer(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end)
{
    auto result = std::shared_ptr<ObjectStreamer>(new ObjectStreamer(std::move(stream), false));
    result->weak_self = result;
    result->write_callback = std::move(end);
    return result;
}

std::shared_ptr<gearoenix::core::ObjectStreamer> gearoenix::core::ObjectStreamer::construct_reader(std::shared_ptr<platform::stream::Stream>&& stream)
{
    auto result = std::shared_ptr<ObjectStreamer>(new ObjectStreamer(std::move(stream), true));
    result->weak_self = result;
    result->read();
    return result;
}

void gearoenix::core::ObjectStreamer::write(std::shared_ptr<Object>&& object)
{
    GX_ASSERT_D(object);
    GX_ASSERT_D(!is_reading);
    const auto id = object->get_object_id();
    std::shared_ptr<platform::stream::Stream> write_stream;
    {
        const std::lock_guard _lg(lock);
        if (objects.contains(id)) {
            return;
        }
        write_stream = std::make_shared<platform::stream::Memory>();
        objects[id] = ObjectInfo { object, write_stream };
    }
    GX_ASSERT_D(!weak_self.expired());
    object->write(std::move(write_stream), weak_self.lock());
}

void gearoenix::core::ObjectStreamer::read(const id_t id, std::function<void(const std::shared_ptr<Object>&)>&& fun)
{
    GX_ASSERT_D(is_reading);
    const std::shared_ptr<Object>* ptr = nullptr;
    {
        const std::lock_guard _lg(lock);
        const auto search = objects.find(id);
        if (objects.end() == search) {
            fun(nullptr);
            return;
        }
        if (nullptr == search->second.object) {
            search->second.read_functions.push_back(std::move(fun));
            return;
        }
        ptr = &search->second.object;
    }
    fun(*ptr);
}
