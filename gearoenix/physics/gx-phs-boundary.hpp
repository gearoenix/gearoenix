#ifndef GEAROENIX_PHYSICS_BOUNDARY_HPP
#define GEAROENIX_PHYSICS_BOUNDARY_HPP
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../math/gx-math-aabb.hpp"

namespace gearoenix::physics {
struct Boundary final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Aabb3<double>, box);

private:
    Boundary(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name);

public:
    Boundary(const Boundary&) = delete;
    Boundary& operator=(Boundary&&) = delete;
    Boundary& operator=(const Boundary&) = delete;
};
}

#endif