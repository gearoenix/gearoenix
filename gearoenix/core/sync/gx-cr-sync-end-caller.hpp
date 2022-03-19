#ifndef GEAROENIX_CORE_SYNC_END_CALLER_HPP
#define GEAROENIX_CORE_SYNC_END_CALLER_HPP
#include "../macro/gx-cr-mcr-assert.hpp"
#include <functional>
#include <memory>
#include <type_traits>

#define GX_DEFAULT_IGNORED_END_CALLER gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>([]() {})
#define GX_DEFAULT_END_CALLER(x) gearoenix::core::sync::EndCaller<x>([](const std::shared_ptr<x>&) {})

namespace gearoenix::core::sync {
template <typename T>
struct EndCaller final {
private:
    struct Caller final {
        const std::function<void(std::shared_ptr<T>)> f;
        std::shared_ptr<T> data;

        Caller(std::function<void(std::shared_ptr<T>)> f) noexcept
            : f(std::move(f))
        {
        }

        ~Caller() noexcept
        {
            GX_CHECK_NOT_EQUAL_D(nullptr, data);
            f(std::move(data));
        }
    };

    std::shared_ptr<Caller> caller;

public:
    EndCaller(std::function<void(std::shared_ptr<T>)> f) noexcept
        : caller(new Caller(std::move(f)))
    {
    }

    EndCaller(const EndCaller&) noexcept = default;
    EndCaller(EndCaller&&) noexcept = default;

    void set_data(std::shared_ptr<T> data) noexcept
    {
        caller->data = std::move(data);
    }
};

typedef struct EndCallerIgnore {
} EndCallerIgnore;

template <>
struct EndCaller<EndCallerIgnore> final {
private:
    struct Caller final {
        const std::function<void()> f;

        Caller(std::function<void()> f) noexcept
            : f(std::move(f))
        {
        }

        ~Caller() noexcept
        {
            f();
        }
    };
    std::shared_ptr<Caller> caller;

public:
    EndCaller(std::function<void()> f) noexcept
        : caller(new Caller(std::move(f)))
    {
    }

    EndCaller(const EndCaller& o) noexcept = default;
    EndCaller(EndCaller&&) noexcept = default;
};

typedef EndCaller<EndCallerIgnore> EndCallerIgnored;
}
#endif
