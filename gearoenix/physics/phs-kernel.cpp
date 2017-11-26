#include "phs-kernel.hpp"
#include <functional>
#include <iostream>

void gearoenix::physics::Kernel::run()
{
    while (alive) {
        std::unique_lock<std::mutex> ul(cvm);
        cv.wait(ul);
        std::string s = std::to_string(thread_index) + " of " + std::to_string(threads_count) + "\n";
        std::cout << s;
    }
    alive = true;
}

gearoenix::physics::Kernel::Kernel(const unsigned int thread_index, const unsigned int threads_count)
    : thread_index(thread_index)
    , threads_count(threads_count)
{
    thread = std::thread(std::bind(&Kernel::run, this));
}

gearoenix::physics::Kernel::~Kernel()
{
    alive = false;
    do {
        cv.notify_one();
        std::this_thread::yield();
    } while (!alive);
    thread.join();
}

void gearoenix::physics::Kernel::signal()
{
    cv.notify_one();
}
