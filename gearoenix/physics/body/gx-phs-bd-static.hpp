#ifndef GEAROENIX_PHYSICS_BODY_STATIC_HPP
#define GEAROENIX_PHYSICS_BODY_STATIC_HPP
#include "gx-phs-bd-body.hpp"

namespace gearoenix::physics::body {
struct Static : public Body {
private:
    double friction;
    double bounciness;

public:
    Static();
    virtual ~Static();
};
}
#endif
