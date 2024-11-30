#pragma once
#include "../platform/gx-plt-build-configuration.hpp"
#ifndef GX_PLATFORM_ANDROID
#include "gx-math-matrix-4d.hpp"
#include "gx-math-numeric.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(gearoenix_math_matrix_4d)
{
    using GxMat4 = gearoenix::math::Mat4x4<float>;
    using GxVec3 = gearoenix::math::Vec3<float>;

    std::default_random_engine re(std::random_device {}());
    std::uniform_real_distribution<float> dis(-1.0, 1.0);

    glm::mat4 glmm;
    GxMat4 gxm;

    auto compare = [&] {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (!gearoenix::math::Numeric::equal(glmm[i][j], gxm[i][j])) {
                    return false;
                }
            }
        }
        return true;
    };

    glmm = glm::translate(glm::rotate(glm::mat4(1.0f), 0.2f, glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0f, 2.0f));
    gxm = GxMat4::rotation({ 0.0f, 0.0f, 1.0f }, 0.2f);
    gxm.translate({ 0.0f, 0.0f, 2.0f });

    BOOST_TEST(compare(), "hard-coded-rotation-translation-1");

    glmm = glm::inverse(glmm);
    gxm.inverse();

    BOOST_TEST(compare(), "hard-coded-inverse-1");

    for (int i = 0; i < 1000; ++i) {
        const auto px = dis(re), py = dis(re), pz = dis(re);
        const auto tx = dis(re), ty = dis(re), tz = dis(re);
        const auto ux = dis(re), uy = dis(re), uz = dis(re);

        glmm = glm::lookAt(glm::vec3(px, py, pz), { tx, ty, tz }, { ux, uy, uz });
        gxm = GxMat4::look_at({ px, py, pz }, { tx, ty, tz }, { ux, uy, uz });

        BOOST_TEST(compare(), "hard-coded-look-at-1-" << i);
    }

    for (int i = 0; i < 1000; ++i) {
        const auto px = dis(re), py = dis(re), pz = dis(re);
        const auto tx = dis(re), ty = dis(re), tz = dis(re);
        const auto ux = dis(re), uy = dis(re), uz = dis(re);

        glmm = glm::lookAt(glm::vec3(px, py, pz), { tx, ty, tz }, { ux, uy, uz });
        gxm = GxMat4::look_at_inverted({ px, py, pz }, { tx, ty, tz }, { ux, uy, uz }).inverted();

        BOOST_TEST(compare(), "hard-coded-look-at-2-" << i);
    }
}
#endif
