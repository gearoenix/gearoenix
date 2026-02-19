#pragma once
#include "gx-math-quaternion.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/mat4x4.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(gearoenix_math_quaternion)
{
    using GxN = gearoenix::math::Numeric;
    using GxMat4 = gearoenix::math::Mat4x4<float>;
    using GxQuat = gearoenix::math::Quat<float>;
    using GxVec3 = gearoenix::math::Vec3<float>;

    constexpr auto rand_dom = 10.0f;

    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_real_distribution dis(-rand_dom, rand_dom);

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x = dis(re);
        const auto y = dis(re);
        const auto z = dis(re);
        const auto w = dis(re);

        const GxQuat gx_quat(x, y, z, w);
        const auto gxm = gx_quat.to_m4x4();

        const glm::quat glm_quat(w, x, y, z);
        const auto glmm = glm::mat4_cast(glm_quat);

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                const auto a = glmm[i][j];
                const auto b = gxm[i][j];
                if (GxN::equal(a, b)) {
                    continue;
                }
                BOOST_TEST_FAIL("Mat4x4 compare failed on [" << i << "][" << j << "], " << a << " != " << b);
            }
        }
    }

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x = dis(re);
        const auto y = dis(re);
        const auto z = dis(re);

        const auto gx_quat = GxQuat::from_euler(x, y, z);
        const auto glm_quat = glm::quat(glm::vec3(x, y, z));

        BOOST_TEST(gx_quat.safe_equal(GxQuat(glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w)), "Quaternion from Euler comparison failed Q { " << x << ", " << y << ", " << z << " }");
    }

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x = dis(re);
        const auto y = dis(re);
        const auto z = dis(re);
        const auto w = dis(re);

        const auto gx_ang = GxQuat(x, y, z, w).to_euler();
        const auto glm_ang = glm::eulerAngles(glm::quat(w, x, y, z));

        BOOST_TEST(gx_ang.equal({ glm_ang.x, glm_ang.y, glm_ang.z }), "Quaternion to Euler comparison failed Q { " << x << ", " << y << ", " << z << ", " << w << " }");
    }

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x0 = dis(re);
        const auto y0 = dis(re);
        const auto z0 = dis(re);
        const auto w0 = dis(re);
        const auto x1 = dis(re);
        const auto y1 = dis(re);
        const auto z1 = dis(re);
        const auto w1 = dis(re);
        const auto x2 = dis(re);
        const auto y2 = dis(re);
        const auto z2 = dis(re);
        const auto w2 = dis(re);
        const auto x3 = dis(re);
        const auto y3 = dis(re);
        const auto z3 = dis(re);
        const auto w3 = dis(re);

        auto m = glm::mat4(1.0f);
        m = glm::rotate(m, glm::radians(w0), glm::vec3(x0, y0, z0));
        m = glm::rotate(m, glm::radians(w1), glm::vec3(x1, y1, z1));
        m = glm::rotate(m, glm::radians(w2), glm::vec3(x2, y2, z2));
        m = glm::rotate(m, glm::radians(w3), glm::vec3(x3, y3, z3));

        const auto gx_m = GxMat4(m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);

        const auto gx_quat = GxQuat::from(gx_m);
        const auto glm_quat = glm::quat_cast(glm::mat4(m));

        BOOST_TEST(gx_quat.safe_equal(GxQuat(glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w)), "Quaternion from matrix comparison failed. " << ti);
    }

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x0 = dis(re);
        const auto y0 = dis(re);
        const auto z0 = dis(re);
        const auto w0 = dis(re);
        const auto x1 = dis(re);
        const auto y1 = dis(re);
        const auto z1 = dis(re);
        const auto w1 = dis(re);
        const auto x2 = dis(re);
        const auto y2 = dis(re);
        const auto z2 = dis(re);
        const auto w2 = dis(re);
        const auto x3 = dis(re);
        const auto y3 = dis(re);
        const auto z3 = dis(re);
        const auto w3 = dis(re);

        const auto gx_quat = GxQuat::from(GxMat4(x0, y0, z0, w0, x1, y1, z1, w1, x2, y2, z2, w2, x3, y3, z3, w3));
        const auto glm_quat = glm::quat_cast(glm::mat4(x0, y0, z0, w0, x1, y1, z1, w1, x2, y2, z2, w2, x3, y3, z3, w3));

        BOOST_TEST(gx_quat.equal(GxQuat(glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w)), "Quaternion from matrix comparison failed.");
    }
    for (auto ti = 0; ti < 100; ++ti) {
        const auto x0 = dis(re);
        const auto y0 = dis(re);
        const auto z0 = dis(re);
        const auto w0 = dis(re);
        const auto x1 = dis(re);
        const auto y1 = dis(re);
        const auto z1 = dis(re);
        const auto w1 = dis(re);

        const auto gx_quat = GxQuat(x0, y0, z0, w0) * GxQuat(x1, y1, z1, w1);
        const auto glm_quat = glm::quat(w0, x0, y0, z0) * glm::quat(w1, x1, y1, z1);

        BOOST_TEST(gx_quat.equal(GxQuat(glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w)), "Quaternion from matrix comparison failed.");
    }

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x0 = dis(re);
        const auto y0 = dis(re);
        const auto z0 = dis(re);
        const auto w0 = dis(re);

        const auto gx_quat = GxQuat::angle_axis(w0, { x0, y0, z0 });
        const auto glm_quat = glm::angleAxis(w0, glm::vec3(x0, y0, z0));

        BOOST_TEST(gx_quat.equal(GxQuat(glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w)), "Quaternion from matrix comparison failed.");
    }

    for (auto ti = 0; ti < 100; ++ti) {
        const auto x0 = dis(re);
        const auto y0 = dis(re);
        const auto z0 = dis(re);
        const auto w0 = dis(re);
        const auto x1 = dis(re);
        const auto y1 = dis(re);
        const auto z1 = dis(re);

        const auto gxq = GxQuat(x0, y0, z0, w0).normalised();
        const auto gxv = GxVec3(x1, y1, z1);

        const auto gxr1 = gxq.rotate(gxv);
        const auto gxr2 = gxq.to_m3x3() * gxv;

        if (!gxr1.equal(gxr2)) {
            BOOST_TEST_FAIL("Matrix rotation is different to the Quaternion rotation");
        }

        const auto glm_q = glm::normalize(glm::quat(w0, x0, y0, z0));
        const auto glm_v = glm::vec3(x1, y1, z1);

        const auto glm_r1 = glm_q * glm_v;
        const auto glm_r2 = glm::mat3_cast(glm_q) * glm_v;

        if (!gxr1.equal({ glm_r1.x, glm_r1.y, glm_r1.z }) || !gxr1.equal({ glm_r2.x, glm_r2.y, glm_r2.z })) {
            BOOST_TEST_FAIL("GLM is different in Quaternion rotation");
        }
    }

    for (auto ti = 0; ti < 1000; ++ti) {
        const auto x0 = dis(re);
        const auto y0 = dis(re);
        const auto z0 = dis(re);
        const auto w0 = dis(re);
        const auto x1 = dis(re);
        const auto y1 = dis(re);
        const auto z1 = dis(re);
        const auto w1 = dis(re);
        const auto x2 = std::abs(dis(re) / rand_dom);

        const auto gxq0 = GxQuat(x0, y0, z0, w0).normalised();
        const auto gxq1 = GxQuat(x1, y1, z1, w1).normalised();
        const auto gxq = gxq0.slerp(gxq1, x2);

        const auto glm_q0 = glm::normalize(glm::quat(w0, x0, y0, z0));
        const auto glm_q1 = glm::normalize(glm::quat(w1, x1, y1, z1));
        const auto glm_q = glm::slerp(glm_q0, glm_q1, x2);

        if (!gxq.safe_equal({ glm_q.x, glm_q.y, glm_q.z, glm_q.w })) {
            BOOST_TEST_FAIL("GLM is different in Quaternion slerp");
        }
    }
}