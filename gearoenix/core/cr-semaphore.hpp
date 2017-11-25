#ifndef GEAROENIX_CORE_SEMAPHORE_HPP
#define GEAROENIX_CORE_SEMAPHORE_HPP
#ifdef IN_WINDOWS
#include <windows.h>
#else
#include <semaphore.h>
#endif

namespace gearoenix {
namespace core {
    class Semaphore {
    private:
#ifdef IN_WINDOWS
        HANDLE sem;
#else
        sem_t sem;
#endif

    public:
        Semaphore(int count = 0);
        ~Semaphore();
        void lock();
        void release();
    };
}
}
#endif
