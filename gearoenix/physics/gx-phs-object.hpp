#ifndef GEAROENIX_PHYSICS_OBJECT_HPP
#define GEAROENIX_PHYSICS_OBJECT_HPP
namespace gearoenix {
namespace physics {
    struct Object {
    public:
        struct PhysicsObject {
        public:
            typedef enum {
                ANIMATION,
                BODY,
                COLLIDER,
                CONSTRAINT,
                TRANSFORMABLE,
                UNTRANSFORMABLE,
            } Type;
        };

    private:
    protected:
        const PhysicsObject::Type physics_object_type;

    public:
        Object(PhysicsObject::Type t);
        virtual ~Object();
        PhysicsObject::Type get_physics_object_type() const;
    };
}
}
#endif