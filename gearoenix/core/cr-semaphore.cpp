#include "cr-semaphore.hpp"

gearoenix::core::Semaphore::Semaphore(int count)
{
#ifdef IN_WINDOWS
    sem = CreateSemaphore(NULL, count, MAXLONG32, NULL);
#else
    sem_init(&sem, 0, count);
#endif
}

gearoenix::core::Semaphore::~Semaphore()
{
#ifdef IN_WINDOWS
    CloseHandle(sem);
#else
    sem_destroy(&sem);
#endif
}

void gearoenix::core::Semaphore::lock()
{
#ifdef IN_WINDOWS
    WaitForSingleObject(sem, INFINITE);
#else
    sem_wait(&sem);
#endif
}

void gearoenix::core::Semaphore::release()
{
#ifdef IN_WINDOWS
    ReleaseSemaphore(sem, 1, NULL);
#else
    sem_post(&sem);
#endif
}
