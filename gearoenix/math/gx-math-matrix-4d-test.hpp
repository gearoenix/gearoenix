#ifndef GEAROENIX_MATH_MATRIX_4D_TEST_HPP
#define GEAROENIX_MATH_MATRIX_4D_TEST_HPP
#include "../platform/gx-plt-build-configuration.hpp"
#ifndef GX_PLATFORM_ANDROID
#include "gx-math-matrix-4d.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

BOOST_AUTO_TEST_CASE(gearoenix_math_matrix_4d)
{
    using GxMat4 = gearoenix::math::Mat4x4<float>;

    glm::mat4 glmm;
    GxMat4 gxm;

    constexpr float epsilon = 0.0001f;

    auto compare = [&](const std::string& operation, const int location) noexcept {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                const auto a = glmm[i][j];
                const auto b = gxm.data[i][j];
                if (a == b)
                    continue;
                const auto abs_a = std::abs(a);
                const auto abs_b = std::abs(b);
                const auto d = std::abs(b - a);
                if ((abs_a < epsilon || abs_b < epsilon) && d < epsilon)
                    continue;
                if (d / (a + b) < d)
                    continue;
                BOOST_TEST_FAIL("Mat4x4 compare failed on [" << i << "][" << j << "] in line: " << location << " in operation: " << operation);
            }
    };

    glmm = glm::translate(glm::rotate(glm::mat4(1.0f), 0.2f, glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0f, 2.0f));
    gxm = GxMat4::rotation({ 0.0f, 0.0f, 1.0f }, 0.2f);
    gxm.translate({ 0.0f, 0.0f, 2.0f });

    compare("hard-coded-rotation-translation-1", __LINE__);

    glmm = glm::inverse(glmm);
    gxm.inverse();

    compare("hard-coded-inverse-1", __LINE__);
}
#endif
#endif
