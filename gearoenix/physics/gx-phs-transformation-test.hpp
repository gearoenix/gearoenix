#ifndef GEAROENIX_PHYSICS_TRANSFORMATION_TEST_HPP
#define GEAROENIX_PHYSICS_TRANSFORMATION_TEST_HPP
#include "gx-phs-transformation.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(gearoenix_physics_transformation)
{
    gearoenix::core::ecs::Component::register_type<gearoenix::physics::Transformation>();

    gearoenix::physics::Transformation transform("transformation");

    auto glmm = glm::identity<glm::dmat4>();

    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_real_distribution dis(-100.0, 100.0);

    for (int j = 0; j < 10; ++j) {
        for (int i = 0; i < 10; ++i) {
            const auto x = dis(re) > 0.0 ? std::abs(dis(re)) + 0.01 : -std::abs(dis(re)) - 0.01;
            const auto y = dis(re);
            const auto z = dis(re);
            const auto d = dis(re);

            transform.local_rotate(d, gearoenix::math::Vec3(x, y, z).normalised());
            glmm = glm::rotate(glm::identity<glm::dmat4>(), d, glm::normalize(glm::dvec3(x, y, z))) * glmm;
        }

        auto gxq = transform.get_local_orientation();
        auto glmq = glm::quat_cast(glmm);

        if (!gxq.safe_equal(gearoenix::math::Quat<double>(glmq.x, glmq.y, glmq.z, glmq.w))) {
            GX_TEST_FLOAT_NEAR(gxq.w, glmq.w);
            GX_TEST_FLOAT_NEAR(gxq.x, glmq.x);
            GX_TEST_FLOAT_NEAR(gxq.y, glmq.y);
            GX_TEST_FLOAT_NEAR(gxq.z, glmq.z);
        }
    }
}
#endif
