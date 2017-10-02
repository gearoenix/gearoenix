#include "cr-end-caller.hpp"

gearoenix::core::EndCaller::EndCaller(std::function<void()> f)
    : f(f)
{
}

void gearoenix::core::EndCaller::add(int c)
{
    std::lock_guard<std::mutex> ll(l);
    w += c;
    (void)ll;
}

void gearoenix::core::EndCaller::done()
{
    l.lock();
    r = true;
    if (0 == w) {
        l.unlock();
        f();
        delete this;
        return;
    }
    l.unlock();
}

void gearoenix::core::EndCaller::remove(int c)
{
    l.lock();
    w -= c;
    if ((0 == w) && r) {
        l.unlock();
        f();
        delete this;
        return;
    }
    l.unlock();
}
