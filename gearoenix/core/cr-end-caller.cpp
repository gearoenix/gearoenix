#include "cr-end-caller.hpp"

gearoenix::core::EndCaller::EndCaller(std::function<void()> f)
    : f(f)
{
}

gearoenix::core::EndCaller::~EndCaller()
{
    f();
}

std::shared_ptr<gearoenix::core::EndCaller> gearoenix::core::EndCaller::create(std::function<void()> f)
{
    return std::shared_ptr<EndCaller>(new EndCaller(f));
}
