#ifndef GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#define GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#include <memory>
#include <mutex>

namespace gearoenix {
namespace core {
    namespace sync {
        class QueuedSemaphore;
        class StopPoint {
        private:
            const std::shared_ptr<QueuedSemaphore> sem;
            const int stoper;
            volatile int counter = 0;
            std::mutex counter_lock;

        public:
            StopPoint(int walkers_count);
            void all_reach();
        };
    }
}
}
#endif
