#include "phs-object.hpp"

gearoenix::physics::Object::Object(PhysicsObject::Type t)
    : physics_object_type(t)
{
}

gearoenix::physics::Object::~Object() { }

gearoenix::physics::Object::PhysicsObject::Type gearoenix::physics::Object::get_physics_object_type() const
{
    return physics_object_type;
}