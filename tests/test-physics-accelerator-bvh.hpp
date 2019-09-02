#include <gearoenix/physics/accelerator/phs-acc-bvh.hpp>
#include <gearoenix/physics/collider/phs-cld-sphere.hpp>

BOOST_AUTO_TEST_CASE(physics_accelerator_bvh)
{
	gearoenix::physics::accelerator::Bvh bvh;

	std::vector<std::unique_ptr<gearoenix::physics::collider::Collider>> cs;
	std::vector<const gearoenix::physics::collider::Collider*> colliders;

	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Sphere>(
			gearoenix::math::Vec3(0.0f, 0.0f, 0.0f), 1.0f));

#define GX_TEST_HELPER { colliders.clear(); for(auto& c: cs) colliders.push_back(c.get()); }

	GX_TEST_HELPER
	bvh.reset(colliders);

	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Sphere>(
			gearoenix::math::Vec3(1.0f, 0.0f, 0.0f), 1.0f));
	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Sphere>(
			gearoenix::math::Vec3(-1.0f, 0.0f, 0.0f), 1.0f));

	GX_TEST_HELPER
	bvh.reset(colliders);

}