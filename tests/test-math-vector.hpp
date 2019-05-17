#include <gearoenix/core/cr-static.hpp>
#include <gearoenix/math/math-aabb.hpp>
#include <gearoenix/math/math-matrix.hpp>
#include <gearoenix/math/math-quaternion.hpp>
#include <gearoenix/math/math-ray.hpp>
#include <gearoenix/math/math-triangle.hpp>
#include <gearoenix/math/math-vector.hpp>
#include <gearoenix/system/sys-log.hpp>
#define GLM_FORCE_CXX14
#define GLM_FORCE_SIMD_SSE42
#define GLM_ENABLE_EXPERIMENTAL
#include <algorithm>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>

static std::random_device rd;
static std::default_random_engine gen(rd());
static std::uniform_real_distribution<> dis(0.0, 1.0);
static long double glmt = 0.0L, gt = 0.0L;

#define PROF(x, t)                                                                                                                  \
    {                                                                                                                               \
        auto now1 = std::chrono::high_resolution_clock::now();                                                                      \
        x;                                                                                                                          \
        auto now2 = std::chrono::high_resolution_clock::now();                                                                      \
        std::chrono::duration<long double> time_span = std::chrono::duration_cast<std::chrono::duration<long double>>(now2 - now1); \
        t += time_span.count();                                                                                                     \
    }

void rm(glm::mat4& m1, gearoenix::math::Mat4x4& m2)
{
    for (unsigned int i = 0; i < 16; ++i) {
        glm::value_ptr(m1)[i] = static_cast<gearoenix::core::Real>(dis(gen));
        m2[i] = glm::value_ptr(m1)[i];
    }
}

void rv(glm::vec3& v1, gearoenix::math::Vec3& v2)
{
    for (int i = 0; i < 3; ++i) {
        v1[i] = static_cast<gearoenix::core::Real>(dis(gen));
        v2[i] = v1[i];
    }
}

void rq(glm::quat& q1, gearoenix::math::Quat& q2)
{
    q1.x = static_cast<gearoenix::core::Real>(dis(gen));
    q2.x = q1.x;
    q1.y = static_cast<gearoenix::core::Real>(dis(gen));
    q2.y = q1.y;
    q1.z = static_cast<gearoenix::core::Real>(dis(gen));
    q2.z = q1.z;
    q1.w = static_cast<gearoenix::core::Real>(dis(gen));
    q2.w = q1.w;
}

void check_eq(const gearoenix::core::Real f1, const gearoenix::core::Real f2)
{
    const float v1 = std::abs(f1 - f2);
    const float v2 = std::abs(f1 + f2);
    if (GXISZERO(v1) && GXISZERO(v2))
        return;
    if (GXISZERO(v2) && GXISNOTZERO(v1))
        BOOST_TEST(false);
    if (f1 / f2 > 0.01f)
        BOOST_TEST(false);
}

void check(glm::mat4& m1, gearoenix::math::Mat4x4& m2)
{
    for (unsigned int i = 0; i < 16; ++i) {
        float f1 = std::abs(glm::value_ptr(m1)[i] - m2[i]);
        float f2 = std::abs(glm::value_ptr(m1)[i] + m2[i]);
        if (f1 > f2)
            std::swap(f1, f2);
        if (f1 / f2 > 0.01f) {
            for (unsigned int j = 0; j < 16; ++j)
                BOOST_TEST_MESSAGE("Error-" << glm::value_ptr(m1)[j] << ", " << m2[j]);
            BOOST_TEST(false);
        }
    }
}

BOOST_AUTO_TEST_CASE(math_vector_test)
{
    glm::mat4 m1;
    gearoenix::math::Mat4x4 m2;
    glm::vec3 v1;
    gearoenix::math::Vec3 v2;
    float f;
    BOOST_TEST_MESSAGE("Rotation test");
    for (int i = 0; i < 1000; ++i) {
        f = static_cast<float>(dis(gen));
        rv(v1, v2);
        v1 = glm::normalize(v1);
        PROF(m1 = glm::rotate(glm::mat4(1.0f), f, v1), glmt);
        PROF(m2 = gearoenix::math::Mat4x4::rotation(v2, f), gt);
        check(m1, m2);
    }
    BOOST_TEST_MESSAGE("Inversion test");
    BOOST_TEST_MESSAGE("glmt " << glmt << ", gt " << gt);
    for (int i = 0; i < 1000; ++i) {
        f = static_cast<float>(dis(gen));
        rm(m1, m2);
        PROF(m1 = glm::inverse(m1), glmt);
        PROF(m2 = m2.inversed(), gt);
        check(m1, m2);
    }
    BOOST_TEST_MESSAGE("glmt " << glmt << ", gt " << gt);
    BOOST_TEST_MESSAGE("Quaternion tests");
    glm::quat q1;
    gearoenix::math::Quat q2;
    for (int i = 0; i < 1000; ++i) {
        rq(q1, q2);
        PROF(m1 = glm::toMat4(q1), glmt);
        PROF(m2 = q2.to_mat(), gt);
        check(m1, m2);
    }
    BOOST_TEST_MESSAGE("glmt " << glmt << ", gt " << gt);
    /////////////////////////////////////////////////////////////
    // for (int i = 0; i < 1000; ++i)
    // {
    //     const gearoenix::math::Vec3 rayo(
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)));
    //     gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
    //     gearoenix::math::Aabb3 aabb(gearoenix::math::Vec3(0.0f, 1.0f, 1.0f), gearoenix::math::Vec3(0.0f, -1.0f, -1.0f));
    //     gearoenix::core::Real tmin;
    //     bool hit = aabb.test(ray, tmin);
    //     if (!hit)
    //     {
    //         GXLOGF("hit: " << hit << ", tmin: " << tmin);
    //     }
    // }
    // for (int i = 0; i < 1000; ++i)
    // {
    //     const gearoenix::math::Vec3 rayo(
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)));
    //     gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
    //     gearoenix::math::Aabb3 aabb(gearoenix::math::Vec3(1.0f, 1.0f, 1001.0f), gearoenix::math::Vec3(-1.0f, -1.0f, 1000.0f));
    //     gearoenix::core::Real tmin;
    //     bool hit = aabb.test(ray, tmin);
    //     if (hit)
    //     {
    //         GXLOGF("hit: " << hit << ", tmin: " << tmin);
    //     }
    // }
    // for (int i = 0; i < 1000; ++i)
    // {
    //     const gearoenix::math::Vec3 rayo(
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)));
    //     gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
    //     gearoenix::math::Triangle3 tri(
    //         gearoenix::math::Vec3(0.0f, 1.0f, -1.0f),
    //         gearoenix::math::Vec3(0.0f, -1.0f, -1.0f),
    //         gearoenix::math::Vec3(0.0f, 0.0f, 1.0f));
    //     gearoenix::math::Vec3 fc;
    //     bool hit = tri.intersect(ray, std::numeric_limits<gearoenix::core::Real>::max(), fc);
    //     if (!hit)
    //     {
    //         GXLOGF("hit: " << hit);
    //     }
    // }
    // for (int i = 0; i < 1000; ++i)
    // {
    //     const gearoenix::math::Vec3 rayo(
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)),
    //         static_cast<float>((dis(gen) > 0.5 ? -1.0 : 1.0) * (dis(gen) * 100.0 + 1.1)));
    //     gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
    //     gearoenix::math::Triangle3 tri(
    //         gearoenix::math::Vec3(1000.0f, 1.0f, -1.0f),
    //         gearoenix::math::Vec3(1000.0f, -1.0f, -1.0f),
    //         gearoenix::math::Vec3(1000.0f, 0.0f, 1.0f));
    //     gearoenix::math::Vec3 fc;
    //     bool hit = tri.intersect(ray, std::numeric_limits<gearoenix::core::Real>::max(), fc);
    //     if (hit)
    //     {
    //         GXLOGF("hit: " << hit);
    //     }
    // }
}
