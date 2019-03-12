#ifndef GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#define GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#include "../cr-build-configuration.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>

namespace gearoenix {
namespace core {
    namespace sync {
        class Semaphore {
        private:
            struct LockData {
                std::mutex m;
                std::condition_variable c;
                volatile bool locked = true;
                volatile bool unlocked = false;
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
}
#endif
