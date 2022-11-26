#ifndef GEAROENIX_CORE_SYNC_END_CALLER_HPP
#define GEAROENIX_CORE_SYNC_END_CALLER_HPP
#include <functional>
#include <memory>

namespace gearoenix::core::sync {
struct EndCaller final {
private:
    struct Caller final {
        const std::function<void()> f;

        explicit Caller(std::function<void()> f) noexcept
            : f(std::move(f))
        {
        }

        ~Caller() noexcept
        {
            f();
        }

        Caller(Caller&&) = delete;
        Caller(const Caller&) = delete;
    };
    std::shared_ptr<Caller> caller;

public:
    explicit EndCaller(std::function<void()> f) noexcept
        : caller(new Caller(std::move(f)))
    {
    }

    EndCaller(const EndCaller& o) noexcept = default;
    EndCaller(EndCaller&&) noexcept = default;
};
}
#endif
