#ifndef GEAROENIX_MATH_QUATERNION_TEST_HPP
#define GEAROENIX_MATH_QUATERNION_TEST_HPP
#include "gx-math-quaternion.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(gearoenix_math_quaternion)
{
    using GxMat4 = gearoenix::math::Mat4x4<float>;
    using GxQuat = gearoenix::math::Quat<float>;
    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_real_distribution dis(-100.0f, 100.0f);

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x = dis(re);
        const auto y = dis(re);
        const auto z = dis(re);
        const auto w = dis(re);
        const GxQuat gx_quat(x, y, z, w);
        const auto gxm = gx_quat.to_mat();

        const glm::quat glm_quat(w, x, y, z);
        const auto glmm = glm::mat4_cast(glm_quat);

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                const auto a = glmm[i][j];
                const auto b = gxm.data[i][j];
                if (a == b)
                    continue;
                const auto d = std::abs(a - b);
                if (d < 0.0001f || d / (std::abs(a) + std::abs(b)) < 0.0001f)
                    continue;
                BOOST_TEST_FAIL("Mat4x4 compare failed on [" << i << "][" << j << "], " << a << " != " << b);
            }
        }
    }
}

#endif