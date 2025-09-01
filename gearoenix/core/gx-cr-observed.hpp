#pragma once
#include <boost/container/flat_map.hpp>
#include <cstdint>
#include <functional>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::core {
template <typename T>
struct Observed final {
    typedef T type;
    typedef std::function<bool(const type&)> listener_t;
    typedef std::uint32_t id_t;

private:
    type value;
    id_t last_id = 0;
    boost::container::flat_map<id_t, listener_t> listeners;

    void notify()
    {
        for (auto f = listeners.begin(); f != listeners.end();) {
            if (!f->second(value)) {
                f = listeners.erase(f);
            } else {
                ++f;
            }
        }
    }

public:
    template <typename... Args>
    explicit Observed(Args&&... args)
        : value(std::forward<Args>(args)...)
    {
    }

    ~Observed()
    {
        if (listeners.empty()) {
            return;
        }
        std::terminate();
    }

    // true means, keep the observer alive
    [[nodiscard]] id_t add_observer(listener_t&& f)
    {
        const auto id = ++last_id;
        listeners[id] = std::move(f);
        return id;
    }

    void remove_observer(const id_t id)
    {
        if (id == 0) {
            return;
        }
        listeners.erase(id);
    }

    void set(const type& new_value)
    {
        value = new_value;
        notify();
    }

    void set(type&& new_value)
    {
        value = std::move(new_value);
        notify();
    }

    Observed& operator=(const type& new_value)
    {
        set(new_value);
        return *this;
    }

    [[nodiscard]] const type& get() const { return value; }

    void write(platform::stream::Stream& stream) const
    {
        value.write(stream);
    }

    void read(platform::stream::Stream& stream)
    {
        value.read(stream);
        notify();
    }

    [[nodiscard]] bool show_debug_gui()
    {
        if (value.show_debug_gui()) {
            notify();
            return true;
        }
        return false;
    }
};
}