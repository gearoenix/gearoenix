#ifndef GEAROENIX_CORE_SYNC_QUEUED_SEMAPHORE_HPP
#define GEAROENIX_CORE_SYNC_QUEUED_SEMAPHORE_HPP
#include "../cr-build-configuration.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>

namespace gearoenix {
namespace core {
    namespace sync {
        class QueuedSemaphore {
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
            volatile int count = 0;

        public:
            QueuedSemaphore(int count = 0);
            ~QueuedSemaphore();
            void lock();
            void release();
        };
    }
}
}
#endif
