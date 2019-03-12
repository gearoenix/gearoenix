#ifndef GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#define GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#include <mutex>
#include <condition_variable>

namespace gearoenix {
namespace core {
namespace sync {
class Semaphore
{
private:
    std::mutex m;
    std::condition_variable cv;
    unsigned int c = 0;
public:
    Semaphore(unsigned int c = 0);
    void signal();
    void wait();
};
}
}
}
#endif
