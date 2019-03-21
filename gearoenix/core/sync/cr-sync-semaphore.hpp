#ifndef GEAROENIX_CORE_SYNC_SIGNALER_HPP
#define GEAROENIX_CORE_SYNC_SIGNALER_HPP
#include <mutex>
#include <condition_variable>

namespace gearoenix {
namespace core {
namespace sync {
	/// It should be used within two threads
	/// One thread must only wait on it and another one must signal it
	/// Wait must happen before signal otherwise ther is no guarantee of expected execution order
	/// Other uses may cause starvation and unordered signalling
class Signaler
{
private:
    std::mutex m;
    std::condition_variable cv;
	volatile bool locked = true;
	volatile bool unlocked = false;
public:
    void signal();
    void wait();
};
}
}
}
#endif
