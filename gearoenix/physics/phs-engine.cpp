#include "phs-engine.hpp"
#include "phs-kernel.hpp"
#include <iostream>

gearoenix::physics::Engine::Engine()
    : number_of_threads(std::thread::hardware_concurrency() > 4 ? std::thread::hardware_concurrency() : 4)
    , kernels(new Kernel*[number_of_threads])
{
    for (unsigned int i = 0; i < number_of_threads; ++i) {
        kernels[i] = new Kernel(i, number_of_threads);
    }
    std::this_thread::yield();
    std::cout << "Threads created.\n";
}

gearoenix::physics::Engine::~Engine()
{
    std::cout << "Threads are gonna be deleted.\n";
    for (unsigned int i = 0; i < number_of_threads; ++i) {
        delete kernels[i];
    }
    delete[] kernels;
}

void gearoenix::physics::Engine::update()
{
    for (unsigned int i = 0; i < number_of_threads; ++i) {
        kernels[i]->signal();
    }
}
