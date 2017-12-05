#ifndef GEAROENIX_CORE_SEMAPHORE_HPP
#define GEAROENIX_CORE_SEMAPHORE_HPP
#include "cr-build-configuration.hpp"
#ifdef THREAD_SUPPORTED
#include <condition_variable>
#include <mutex>
#include <queue>

namespace gearoenix {
namespace core {
    class Semaphore {
    private:
        struct LockData {
            std::mutex m;
            std::condition_variable c;
            volatile bool locked = true;
        };
        typedef std::shared_ptr<LockData> Lock;
        std::mutex m_count;
        std::queue<Lock> q;
        volatile int count;

    public:
        Semaphore(int count = 0);
        ~Semaphore();
        void lock();
        void release();
    };
}
}
#endif
#endif
