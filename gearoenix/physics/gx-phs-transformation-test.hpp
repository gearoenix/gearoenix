#pragma once
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/gizmo/gx-rnd-gzm-manager.hpp"
#include "gx-phs-engine.hpp"
#include "gx-phs-transformation.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(gearoenix_physics_transformation)
{
    gearoenix::core::ecs::World world;
    {
        const auto gizo_manager = std::make_unique<gearoenix::render::gizmo::Manager>();
        (void)gizo_manager; // We need this to be available.

        gearoenix::core::ecs::ComponentType::add<gearoenix::physics::Transformation>();

        auto entity = gearoenix::core::ecs::Entity::construct("entity", nullptr);

        gearoenix::physics::Transformation transform(entity.get(), "transform");
        constexpr auto gx_epsilon = gearoenix::math::Numeric::epsilon<double>;

        auto glmm = glm::identity<glm::dmat4>();

        std::random_device rd;
        std::default_random_engine re(rd());
        std::uniform_real_distribution dis(-100.0, 100.0);

        world.update();

        for (int j = 0; j < 10; ++j) {
            for (int i = 0; i < 10; ++i) {
                const auto random = dis(re);
                const auto x = std::copysign(std::abs(dis(re)) + gx_epsilon, random);
                const auto y = dis(re);
                const auto z = dis(re);
                const auto d = dis(re);

                transform.local_inner_rotate(d, gearoenix::math::Vec3(x, y, z).normalised());
                glmm = glm::rotate(glm::identity<glm::dmat4>(), d, glm::normalize(glm::dvec3(x, y, z))) * glmm;
            }

            auto gxq = transform.get_rotation().get_quat();
            auto glmq = glm::quat_cast(glmm);

            if (!gxq.safe_equal(gearoenix::math::Quat(glmq.x, glmq.y, glmq.z, glmq.w))) {
                GX_TEST_FLOAT_NEAR(gxq.w, glmq.w);
                GX_TEST_FLOAT_NEAR(gxq.x, glmq.x);
                GX_TEST_FLOAT_NEAR(gxq.y, glmq.y);
                GX_TEST_FLOAT_NEAR(gxq.z, glmq.z);
            }
        }
        world.update();

        for (int tests_count = 0; tests_count < 100; ++tests_count) {
            const auto tran = glm::dvec3(dis(re), dis(re), dis(re));
            const auto rot = glm::normalize(glm::dquat(dis(re), dis(re), dis(re), dis(re)));
            const auto scale = glm::abs(glm::dvec3(dis(re), dis(re), dis(re))) + gx_epsilon;

            glmm = glm::translate(glm::dmat4(1.0), tran) * glm::mat4_cast(rot) * glm::scale(glm::dmat4(1.0), scale);

            gearoenix::math::Mat4x4<double> local;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    local[i][j] = glmm[i][j];
                }
            }

            transform.set_local_matrix(local);

            const auto gxt = transform.get_local_position();
            const auto gxs = transform.get_scale();
            const auto gxq = transform.get_rotation().get_quat();

            BOOST_TEST_CHECK(gxt.equal({ tran.x, tran.y, tran.z }));
            BOOST_TEST_CHECK(gxs.equal({ scale.x, scale.y, scale.z }));
            BOOST_TEST_CHECK(gxq.safe_equal({ rot.x, rot.y, rot.z, rot.w }));
        }
        world.update();
    }

    world.update();
}