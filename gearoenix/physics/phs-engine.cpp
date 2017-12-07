#include "phs-engine.hpp"
#include "../core/cr-semaphore.hpp"
#include "phs-kernel.hpp"
#include <iostream>

#ifdef THREAD_SUPPORTED
gearoenix::physics::Engine::Engine(render::Engine* rndeng)
    : render_engine(rndeng)
    , signaller(new core::Semaphore())
{
    // because of some compiler &/| std problems it is here instead of initializer list
    const_cast<unsigned int&>(threads_count) = std::thread::hardware_concurrency() > 4 ? std::thread::hardware_concurrency() : 4;
    kernels = new Kernel*[threads_count];

    for (unsigned int i = 0; i < threads_count; ++i) {
        kernels[i] = new Kernel(i, this);
    }
    std::this_thread::yield();
    std::cout << "\nThreads created.\n";
}
#else
gearoenix::physics::Engine::Engine(render::Engine* rndeng)
    : render_engine(rndeng)
    , kernel(new Kernel(this))
{
}
#endif

gearoenix::physics::Engine::~Engine()
{
#ifdef THREAD_SUPPORTED
    std::cout << "\nThreads are gonna be deleted.\n";
    for (unsigned int i = 0; i < threads_count; ++i) {
        delete kernels[i];
    }
    delete[] kernels;
    delete signaller;
#else
    delete kernel;
#endif
}

void gearoenix::physics::Engine::update()
{
#ifdef THREAD_SUPPORTED
    for (unsigned int i = 0; i < threads_count; ++i) {
        kernels[i]->signal();
    }
#endif
}

void gearoenix::physics::Engine::wait()
{
#ifdef THREAD_SUPPORTED
    for (unsigned int i = 0; i < threads_count; ++i) {
        signaller->lock();
    }
//    std::cout << "physics update ended.\n";
#else
    kernel->signal();
#endif
}
