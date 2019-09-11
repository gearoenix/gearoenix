#include <gearoenix/physics/accelerator/phs-acc-bvh.hpp>
#include <gearoenix/physics/collider/phs-cld-sphere.hpp>
#include <gearoenix/physics/collider/phs-cld-aabb.hpp>
#include <gearoenix/math/math-ray.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(physics_accelerator_bvh)
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<gearoenix::core::Real> dis1(0.5f, 0.9f);
    std::uniform_real_distribution<gearoenix::core::Real> dis2(-20.0f, 20.0f);

	gearoenix::physics::accelerator::Bvh bvh;

	std::vector<std::unique_ptr<gearoenix::physics::collider::Collider>> cs;
	std::vector<gearoenix::physics::collider::Collider*> colliders;

	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Sphere>(
			gearoenix::math::Vec3(0.0f, 0.0f, 0.0f), 0.9f));

#define GX_TEST_HELPER { colliders.clear(); for(auto& c: cs) colliders.push_back(c.get()); }

	GX_TEST_HELPER
	bvh.reset(colliders);
    GXLOGD(bvh.to_string());

#define GX_TEST_HELPER2(x) {                                    \
		const gearoenix::math::Vec3 p(x, 0.0f, 10.0f);          \
		const gearoenix::math::Vec3 d(0.0f, 0.0f, -1.0f);       \
		const gearoenix::math::Ray3 r(p, d);                    \
		const auto hit = bvh.hit(r,                             \
			std::numeric_limits<gearoenix::core::Real>::max()); \
		BOOST_TEST((*hit).first < 10.1f);                       \
		BOOST_TEST((*hit).first > 9.00f);                       \
	}

#define GX_TEST_HELPER3(x) {                                    \
		const gearoenix::math::Vec3 p(x, 0.0f, 10.0f);          \
		const gearoenix::math::Vec3 d(0.0f, 0.0f, -1.0f);       \
		const gearoenix::math::Ray3 r(p, d);                    \
		const auto hit = bvh.hit(r,                             \
			std::numeric_limits<gearoenix::core::Real>::max()); \
		BOOST_TEST(!hit.has_value());                           \
	}

	GX_TEST_HELPER2(0.0f);
	GX_TEST_HELPER2(0.89f);
	GX_TEST_HELPER2(-0.89f);

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(2.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(1.0f, 0.0f, 0.0f), 0.9f));
	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Sphere>(
			gearoenix::math::Vec3(-1.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(-2.0f, 0.0f, 0.0f), 0.9f));

	GX_TEST_HELPER
	bvh.reset(colliders);
    GXLOGD(bvh.to_string());

    GX_TEST_HELPER2(1.11f);
    GX_TEST_HELPER2(2.00f);
    GX_TEST_HELPER2(2.89f);
	GX_TEST_HELPER2(1.0f);
	GX_TEST_HELPER2(-1.0f);
    GX_TEST_HELPER2(-1.11f);
    GX_TEST_HELPER2(-2.0f);
    GX_TEST_HELPER2(-2.89f);

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(4.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(3.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(-3.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(-4.0f, 0.0f, 0.0f), 0.9f));

    GX_TEST_HELPER
    bvh.reset(colliders);
    GXLOGD(bvh.to_string());

	GX_TEST_HELPER2(4.0f);
	GX_TEST_HELPER2(3.0f);
	GX_TEST_HELPER2(-3.0f);
	GX_TEST_HELPER2(-4.0f);

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0f, 0.0f, 0.0f), 0.9f));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0f, 0.0f, 0.0f), 0.9f));

	GX_TEST_HELPER
		bvh.reset(colliders);
	GXLOGD(bvh.to_string());

	GX_TEST_HELPER2(0.0f);
	GX_TEST_HELPER2(0.0f);
	GX_TEST_HELPER2(0.0f);
	GX_TEST_HELPER2(0.0f);

	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Aabb>(
			gearoenix::math::Vec3(7.0f, 0.0f, 0.9f), 
			gearoenix::math::Vec3(5.0f, 0.0f, 0.0f)));
	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Aabb>(
			gearoenix::math::Vec3(8.0f, 0.0f, 0.9f),
			gearoenix::math::Vec3(6.0f, 0.0f, 0.0f)));
	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Aabb>(
			gearoenix::math::Vec3(9.0f, 0.0f, 0.9f),
			gearoenix::math::Vec3(7.0f, 0.0f, 0.0f)));
	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Aabb>(
			gearoenix::math::Vec3(10.0f, 0.0f, 0.9f),
			gearoenix::math::Vec3(8.0f, 0.0f, 0.0f)));
	cs.push_back(
		std::make_unique<gearoenix::physics::collider::Aabb>(
			gearoenix::math::Vec3(11.0f, 0.0f, 0.9f),
			gearoenix::math::Vec3(9.0f, 0.0f, 0.0f)));

    GX_TEST_HELPER
    bvh.reset(colliders);
    GXLOGD(bvh.to_string());

	GX_TEST_HELPER2(6.0f);
	GX_TEST_HELPER2(7.0f);
	GX_TEST_HELPER2(8.0f);
	GX_TEST_HELPER2(9.0f);
    GX_TEST_HELPER2(10.0f);

    GX_TEST_HELPER3(12.0f);
    GX_TEST_HELPER3(13.0f);
    GX_TEST_HELPER3(14.0f);
    GX_TEST_HELPER3(15.0f);
    GX_TEST_HELPER3(-7.0f);
    GX_TEST_HELPER3(-8.0f);

    for (int i = 0; i < 2000; ++i)
    {
        cs.push_back(
            std::make_unique<gearoenix::physics::collider::Sphere>(
                gearoenix::math::Vec3(dis2(gen), dis2(gen), dis2(gen)), dis1(gen)));
    }

    GX_TEST_HELPER
    auto now1 = std::chrono::high_resolution_clock::now();
    bvh.reset(colliders);
    auto now2 = std::chrono::high_resolution_clock::now();
    GXLOGD("dis-time: " << (now2 - now1).count());
    GXLOGD(bvh.to_string());

    long double dis_time = 0.0L;

    constexpr int hits_count = 100000;

    for (int i = 0; i < hits_count; ++i) {
        const gearoenix::math::Vec3 p(dis2(gen), dis2(gen), dis2(gen));
        const gearoenix::math::Vec3 t(dis2(gen), dis2(gen), dis2(gen));
        const gearoenix::math::Ray3 r(p, t.normalized());
        now1 = std::chrono::high_resolution_clock::now();
        bvh.hit(r, std::numeric_limits<gearoenix::core::Real>::max());
        now2 = std::chrono::high_resolution_clock::now();
        dis_time += (now2 - now1).count();
    }
    GXLOGD("dis-time for " << hits_count << " hits: " << dis_time);
}