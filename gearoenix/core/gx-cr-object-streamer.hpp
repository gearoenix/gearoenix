#pragma once
#include "job/gx-cr-job-end-caller.hpp"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::core {
struct Object;

/**
 * @class ObjectStreamer
 * @brief Manages the context for a streaming
 *
 * The StreamContext class provides a way to prevent unnecessary copying of the objects like texture and mesh in a
 * stream.
 */
struct ObjectStreamer final {
    typedef std::uint32_t id_t;

    struct ObjectInfo final {
        std::shared_ptr<Object> object;
        std::shared_ptr<platform::stream::Stream> write_stream;
        std::vector<std::function<void(const std::shared_ptr<Object>&)>> read_functions;
    };

private:
    std::weak_ptr<ObjectStreamer> self;
    std::mutex lock;
    std::map<id_t, ObjectInfo> objects;
    const std::shared_ptr<platform::stream::Stream> stream;
    const bool is_reading;

    ObjectStreamer(std::shared_ptr<platform::stream::Stream>&&, bool is_reading);
    void initialise(std::optional<job::EndCaller<>>&& read_callback);

public:
    ObjectStreamer(const ObjectStreamer&) = delete;
    ~ObjectStreamer();
    [[nodiscard]] static std::shared_ptr<ObjectStreamer> construct(
        std::shared_ptr<platform::stream::Stream>&& stream,
        std::optional<job::EndCaller<>>&& read_callback = std::nullopt);
    // it will send to other thread
    void write(std::shared_ptr<Object>&& object);
    void write(platform::stream::Stream& stream);
    void read(id_t, std::function<void(const std::shared_ptr<Object>&)>&&);
};
}