#include "cr-gc-range.hpp"

gearoenix::core::gc::Range::Range(list::Node<Object*>* start, list::Node<Object*>* end)
    : start(start)
    , end(end)
{
}

gearoenix::core::gc::Range::Range() = default;
