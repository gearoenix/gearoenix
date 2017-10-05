#include "cr-end-caller.hpp"

gearoenix::core::EndCaller::EndCaller(std::function<void()> f)
    : f(f)
{
}

gearoenix::core::EndCaller::~EndCaller()
{
    f();
}
