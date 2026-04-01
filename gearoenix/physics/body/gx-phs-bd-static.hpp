#pragma once
#include "gx-phs-bd-body.hpp"

namespace gearoenix::physics::body {
struct Static : public Body {
private:
    core::fp_t friction;
    core::fp_t bounciness;

public:
    Static();
    virtual ~Static();
};
}