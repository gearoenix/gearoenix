#include "cr-gc.hpp"

gearoenix::core::gc::Gc::Gc(int size, Gc* parent)
    : GcObject(size, parent)
{
}

gearoenix::core::gc::Gc::Gc(int size)
    : GcObject(size)
{
}

gearoenix::core::gc::Gc::~Gc()
{
}
