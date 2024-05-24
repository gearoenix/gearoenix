#ifndef GEAROENIX_CORE_JOB_END_CALLER_HPP
#define GEAROENIX_CORE_JOB_END_CALLER_HPP
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "gx-cr-job-manager.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>

#if GX_DEBUG_MODE
#define GX_END_CALLER_CATCH_CALLER_LOCATION true
#endif

#if GX_END_CALLER_CATCH_CALLER_LOCATION
#include <source_location>

#define GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD(a) a
#define GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(a0, a1) a0, a1

#else

#define GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD(a)
#define GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(a0, a1)

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
        GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD(const std::source_location source_location;)

        explicit Caller(Function&& f GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, const std::source_location& source_location))
            : function(std::move(f))
            , context_thread(std::this_thread::get_id())
                  GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, source_location(source_location))
        {
        }

        ~Caller()
        {
            GX_ASSERT_D(nullptr != this);
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

public:
    explicit EndCaller(Function&& f GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, const std::source_location& source_location = std::source_location::current()))
        : caller(new Caller(std::move(f) GX_END_CALLER_CATCH_CALLER_LOCATION_GUARD2(, source_location)))
    {
    }

    EndCaller(const EndCaller& o) = default;
    EndCaller(EndCaller&&) noexcept = default;

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
#endif
