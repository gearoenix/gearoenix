#ifndef GEAROENIX_PHYSICS_KERNEL_HPP
#define GEAROENIX_PHYSICS_KERNEL_HPP
#include <condition_variable>
#include <mutex>
#include <thread>
#include <tuple>
#include <vector>
namespace gearoenix {
namespace physics {
    class Kernel {
    private:
        const unsigned int thread_index;
        const unsigned int threads_count;
        std::condition_variable cv;
        std::mutex cvm;
        std::thread thread;
        volatile bool alive = true;
        void run();

    protected:
    public:
        Kernel(const unsigned int thread_index, const unsigned int threads_count);
        ~Kernel();
        void signal();
    };
}
}
#endif
