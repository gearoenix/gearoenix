#pragma once
#include "../platform/gx-plt-build-configuration.hpp"
#ifndef GX_PLATFORM_ANDROID
#include "gx-math-matrix-3d.hpp"
#include "gx-math-numeric.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(gearoenix_math_matrix_3d)
{
    using GxMat3 = gearoenix::math::Mat3x3<float>;
    using GxVec3 = gearoenix::math::Vec3<float>;
    using GxN = gearoenix::math::Numeric;

    std::default_random_engine re(std::random_device {}());
    std::uniform_real_distribution<float> dis(-1.0, 1.0);

    glm::mat3 glmm;
    GxMat3 gxm;

    // dodo determinanant

    const auto equal = [&] {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (!GxN::equal(glmm[i][j], gxm[i][j])) {
                    return false;
                }
            }
        }
        return true;
    };

    const auto random = [&] {
        std::array<std::array<float, 3>, 3> values {};
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                values[i][j] = dis(re);
            }
        }
        return values;
    };

    for (int i = 0; i < 100; ++i) {
        const auto vs = random();
        glmm = glm::mat3(vs[0][0], vs[0][1], vs[0][2], vs[1][0], vs[1][1], vs[1][2], vs[2][0], vs[2][1], vs[2][2]);
        gxm = GxMat3(vs[0][0], vs[0][1], vs[0][2], vs[1][0], vs[1][1], vs[1][2], vs[2][0], vs[2][1], vs[2][2]);

        BOOST_TEST(GxN::equal(glm::determinant(glmm), gxm.determinant()));
    }
}
#endif