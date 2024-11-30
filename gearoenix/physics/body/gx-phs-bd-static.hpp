#pragma once
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