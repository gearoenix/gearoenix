#include "cr-gc-object.hpp"
#include "cr-gc.hpp"

gearoenix::core::gc::Object::Object(unsigned int size)
    : size(size)
    , end(size)
{
}

gearoenix::core::gc::Object::~Object()
{
    if (garbage_collector != nullptr) {
        garbage_collector->deallocate(this);
    }
}
