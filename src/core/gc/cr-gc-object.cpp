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

unsigned int gearoenix::core::gc::Object::get_offset() const
{
    return offset;
}

unsigned int gearoenix::core::gc::Object::get_size() const
{
    return size;
}

unsigned int gearoenix::core::gc::Object::get_end() const
{
    return end;
}
