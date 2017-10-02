#ifndef GEAROENIX_CORE_END_CALLER_HPP
#define GEAROENIX_CORE_END_CALLER_HPP
#include <functional>
#include <mutex>
namespace gearoenix {
namespace core {
    class EndCaller {
    private:
        std::mutex l;
        bool r = false;
        std::function<void()> f;
        int w = 0;

    public:
        EndCaller(std::function<void()> f);
        void add(int c = 1);
        void done();
        void remove(int c = 1);
    };
}
}
#endif
