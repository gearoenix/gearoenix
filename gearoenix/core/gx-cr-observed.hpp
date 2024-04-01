#ifndef GEAROENIX_CORE_OBSERVED_HPP
#define GEAROENIX_CORE_OBSERVED_HPP
#include <boost/container/flat_map.hpp>
#include <functional>

namespace gearoenix::core {
template <typename T>
struct Observed final {
private:
    T value;
    std::size_t last_id = 0;
    boost::container::flat_map<std::size_t, std::function<bool(const T&)>> listeners;

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
        if (listeners.empty())
            return;
        std::terminate();
    }

    [[nodiscard]] std::size_t add_observer(std::function<bool(const T&)>&& f)
    {
        const auto id = ++last_id;
        listeners.emplace(id, std::move(f));
        return id;
    }

    void remove_listener(const std::size_t id)
    {
        if (id == 0)
            return;
        listeners.erase(id);
    }

    void set(const T& new_value)
    {
        value = new_value;
        notify();
    }

    void set(T&& new_value)
    {
        value = std::move(new_value);
        notify();
    }

    [[nodiscard]] const T& get() const { return value; }
};
}

#endif