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
    using namespace gearoenix;

    core::ecs::World world;
    {
        core::Object::register_type<physics::Transformation>();

        const auto gizo_manager = std::make_unique<render::gizmo::Manager>();
        (void)gizo_manager; // We need this to be available.

        core::ecs::ComponentType::add<physics::Transformation>();

        auto entity = core::ecs::Entity::construct("entity", nullptr);

        auto transform = core::Object::construct<physics::Transformation>(entity.get(), "transform");
        constexpr auto gx_epsilon = math::Numeric::epsilon<core::fp_t>;

        auto glmm = glm::identity<glm::dmat4>();

        std::random_device rd;
        std::default_random_engine re(rd());
        std::uniform_real_distribution dis(static_cast<core::fp_t>(-100), static_cast<core::fp_t>(100));

        world.update();

        for (int j = 0; j < 10; ++j) {
            for (int i = 0; i < 10; ++i) {
                const auto random = dis(re);
                const auto x = std::copysign(std::abs(dis(re)) + gx_epsilon, random);
                const auto y = dis(re);
                const auto z = dis(re);
                const auto d = dis(re);

                transform->local_inner_rotate(d, math::Vec3(x, y, z).normalised());
                glmm = glm::rotate(glm::identity<glm::dmat4>(), static_cast<double>(d), glm::normalize(glm::dvec3(x, y, z))) * glmm;
            }

            auto gxq = transform->get_rotation().get_quat();
            auto glmq = glm::quat_cast(glmm);

            if (!gxq.safe_equal(math::Quat(glmq.x, glmq.y, glmq.z, glmq.w).to<core::fp_t>())) {
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
            const auto scale = glm::abs(glm::dvec3(dis(re), dis(re), dis(re))) + static_cast<double>(gx_epsilon);

            glmm = glm::translate(glm::dmat4(1.0), tran) * glm::mat4_cast(rot) * glm::scale(glm::dmat4(1.0), scale);

            math::Mat4x4<core::fp_t> local;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    local[i][j] = static_cast<core::fp_t>(glmm[i][j]);
                }
            }

            transform->set_local_matrix(local);

            const auto gxt = transform->get_local_position();
            const auto gxs = transform->get_scale();
            const auto gxq = transform->get_rotation().get_quat();

            BOOST_TEST_CHECK(gxt.equal(math::Vec3(tran.x, tran.y, tran.z).to<core::fp_t>()));
            BOOST_TEST_CHECK(gxs.equal(math::Vec3(scale.x, scale.y, scale.z).to<core::fp_t>()));
            BOOST_TEST_CHECK(gxq.safe_equal(math::Quat(rot.x, rot.y, rot.z, rot.w).to<core::fp_t>()));
        }
        world.update();
    }

    world.update();
}