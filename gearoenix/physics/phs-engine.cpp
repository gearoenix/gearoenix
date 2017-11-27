#include "phs-engine.hpp"
#include "../core/cr-semaphore.hpp"
#include "phs-kernel.hpp"
#include <iostream>

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
    std::cout << "Threads created.\n";
}

gearoenix::physics::Engine::~Engine()
{
    std::cout << "Threads are gonna be deleted.\n";
    for (unsigned int i = 0; i < threads_count; ++i) {
        delete kernels[i];
    }
    delete[] kernels;
}

void gearoenix::physics::Engine::update()
{
    for (unsigned int i = 0; i < threads_count; ++i) {
        kernels[i]->signal();
    }
}
