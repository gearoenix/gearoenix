#include "test-config.hpp"
#ifdef TEST003
#include "../math/math-aabb.hpp"
#include "../math/math-matrix.hpp"
#include "../math/math-ray.hpp"
#include "../math/math-triangle.hpp"
#include "../math/math-vector.hpp"
#include "../system/sys-log.hpp"
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>

std::random_device rd;
std::default_random_engine gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);
long double glmt = 0.0, gt = 0.0;

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
    for (int i = 0; i < 16; ++i) {
        glm::value_ptr(m1)[i] = dis(gen);
        m2[i] = glm::value_ptr(m1)[i];
    }
}

void rv(glm::vec3& v1, gearoenix::math::Vec3& v2)
{
    for (int i = 0; i < 3; ++i) {
        v1[i] = dis(gen);
        v2[i] = v1[i];
    }
}

void check(glm::mat4& m1, gearoenix::math::Mat4x4& m2)
{
    for (int i = 0; i < 16; ++i) {
        float f1 = std::abs(glm::value_ptr(m1)[i] - m2[i]);
        float f2 = std::abs(glm::value_ptr(m1)[i] + m2[i]);
        if (f1 / f2 > 0.01f) {
            for (int j = 0; j < 16; ++j)
                GXLOGE(glm::value_ptr(m1)[j] << ", " << m2[j]);
            UNEXPECTED;
        }
    }
}

int main()
{
    glm::mat4 m1;
    gearoenix::math::Mat4x4 m2;
    glm::vec3 v1, v3;
    gearoenix::math::Vec3 v2, v4;
    float f;
    GXLOGI("Rotation test");
    for (int i = 0; i < 1000; ++i) {
        f = dis(gen);
        rv(v1, v2);
        PROF(m1 = glm::rotate(glm::mat4(), f, v1), glmt);
        PROF(m2 = gearoenix::math::Mat4x4::rotation(v2, f), gt);
        check(m1, m2);
    }
    GXLOGI("Inversion test");
    GXLOGI("glmt " << glmt << ", gt " << gt);
    for (int i = 0; i < 1000; ++i) {
        f = dis(gen);
        rm(m1, m2);
        PROF(m1 = glm::inverse(m1), glmt);
        PROF(m2 = m2.inversed(), gt);
        check(m1, m2);
    }
    GXLOGI("glmt " << glmt << ", gt " << gt);
    /////////////////////////////////////////////////////////////
    for (int i = 0; i < 1000; ++i) {
        const gearoenix::math::Vec3 rayo(
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f));
        gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
        gearoenix::math::Aabb3 aabb(gearoenix::math::Vec3(1.0f, 1.0f, 1.0f), gearoenix::math::Vec3(-1.0f, -1.0f, -1.0f));
        gearoenix::core::Real tmin;
        bool hit = aabb.test(ray, tmin);
        if (!hit) {
            GXLOGF("hit: " << hit << ", tmin: " << tmin);
        }
    }
    for (int i = 0; i < 1000; ++i) {
        const gearoenix::math::Vec3 rayo(
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f));
        gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
        gearoenix::math::Aabb3 aabb(gearoenix::math::Vec3(1.0f, 1.0f, 1001.0f), gearoenix::math::Vec3(-1.0f, -1.0f, 1000.0f));
        gearoenix::core::Real tmin;
        bool hit = aabb.test(ray, tmin);
        if (hit) {
            GXLOGF("hit: " << hit << ", tmin: " << tmin);
        }
    }
    for (int i = 0; i < 1000; ++i) {
        const gearoenix::math::Vec3 rayo(
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f));
        gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
        gearoenix::math::Triangle3 tri(
            gearoenix::math::Vec3(0.0f, 1.0f, -1.0f),
            gearoenix::math::Vec3(0.0f, -1.0f, -1.0f),
            gearoenix::math::Vec3(0.0f, 0.0f, 1.0f));
        gearoenix::math::Vec3 fc;
        bool hit = tri.intersect(ray, std::numeric_limits<gearoenix::core::Real>::max(), fc);
        if (!hit) {
            GXLOGF("hit: " << hit);
        }
    }
    for (int i = 0; i < 1000; ++i) {
        const gearoenix::math::Vec3 rayo(
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f),
            (dis(gen) > 0.5f ? -1 : 1) * (dis(gen) * 100.0f + 1.1f));
        gearoenix::math::Ray3 ray(rayo, (-rayo).normalized());
        gearoenix::math::Triangle3 tri(
            gearoenix::math::Vec3(1000.0f, 1.0f, -1.0f),
            gearoenix::math::Vec3(1000.0f, -1.0f, -1.0f),
            gearoenix::math::Vec3(1000.0f, 0.0f, 1.0f));
        gearoenix::math::Vec3 fc;
        bool hit = tri.intersect(ray, std::numeric_limits<gearoenix::core::Real>::max(), fc);
        if (hit) {
            GXLOGF("hit: " << hit);
        }
    }
}
#endif
