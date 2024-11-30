#pragma once
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "gx-cr-job-manager.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

#if GX_DEBUG_MODE
#define GX_END_CALLER_CATCH_CALLER_LOCATION true
#endif

#if GX_END_CALLER_CATCH_CALLER_LOCATION
#include <boost/stacktrace.hpp>

#define GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD(a) a
#define GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(a1, a2) a1, a2

#else

#define GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD(a)

#endif

namespace gearoenix::core::job {

template <typename T = void>
struct EndCaller final {
private:
    struct Empty { };
    typedef std::conditional_t<std::is_same_v<void, T>, Empty, T> Type;
    typedef std::conditional_t<std::is_same_v<void, T>, std::function<void()>, std::function<void(Type&&)>> Function;

    struct Caller final {
        Function function;
        const std::thread::id context_thread;
        std::optional<Type> value = std::nullopt;
        GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD(const boost::stacktrace::stacktrace stack_trace;)

        explicit Caller(Function&& f)
            : function(std::move(f))
            , context_thread(std::this_thread::get_id())
                  GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, stack_trace(boost::stacktrace::stacktrace()))
        {
        }

        ~Caller()
        {
            if constexpr (std::is_same_v<void, T>) {
                send_job(context_thread, [f = std::move(function)]() mutable {
                    f();
                });
            } else {
                if (value.has_value()) {
                    send_job(context_thread, [v = std::move(*value), f = std::move(function)]() mutable {
                        f(std::move(v));
                    });
                } else {
                    GX_LOG_F(typeid(EndCaller).name() << " is called without value.");
                }
            }
        }

        Caller(Caller&&) = delete;
        Caller(const Caller&) = delete;
    };

    std::shared_ptr<Caller> caller;
    GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD(const boost::stacktrace::stacktrace stack_trace;)

public:
    explicit EndCaller(Function&& f)
        : caller(new Caller(std::move(f)))
              GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, stack_trace(boost::stacktrace::stacktrace()))
    {
    }

    EndCaller(const EndCaller<T>& o)
        : caller(o.caller)
              GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, stack_trace(boost::stacktrace::stacktrace()))
    {
        GX_ASSERT_D(nullptr != caller);
    }

    EndCaller(EndCaller<T>&& o) noexcept
        : caller(std::move(o.caller))
              GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, stack_trace(boost::stacktrace::stacktrace()))
    {
        GX_ASSERT_D(nullptr != caller);
    }

#ifdef GX_DEBUG_MODE
    ~EndCaller()
    {
        caller = nullptr;
    }
#else
    ~EndCaller() = default;
#endif

    template <typename TT = T>
    std::enable_if_t<!std::is_same_v<void, TT>, void> set_return(Type&& v) const
    {
        GX_ASSERT(!caller->value.has_value());
        caller->value = std::move(v);
    }

    template <typename TT = T>
    std::enable_if_t<!std::is_same_v<void, TT>, Type&> get_return() const
    {
        GX_ASSERT(caller->value.has_value());
        return *caller->value;
    }
};

template <typename T>
using EndCallerShared = EndCaller<std::shared_ptr<T>>;

}