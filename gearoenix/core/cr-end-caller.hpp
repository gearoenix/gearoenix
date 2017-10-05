#ifndef GEAROENIX_CORE_END_CALLER_HPP
#define GEAROENIX_CORE_END_CALLER_HPP
#include <functional>
namespace gearoenix {
namespace core {
    class EndCaller {
    private:
        std::function<void()> f;

    public:
        EndCaller(std::function<void()> f);
        ~EndCaller();
    };
}
}
#endif
