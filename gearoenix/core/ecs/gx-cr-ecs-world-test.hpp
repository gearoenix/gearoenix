#ifndef GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#define GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#include "gx-cr-ecs-world.hpp"

struct Position final : public gearoenix::core::ecs::Component {
    double x;
    double y;

    Position(const double x, const double y) noexcept
        : gearoenix::core::ecs::Component(this)
        , x(x)
        , y(y)
    {
    }

    Position(Position&&) noexcept = default;
    Position(const Position&) = delete;
    Position& operator=(Position&&) = delete;
    Position& operator=(const Position&) = delete;
};

struct Speed final : public gearoenix::core::ecs::Component {
    double x;
    double y;

    Speed(const double x, const double y) noexcept
        : gearoenix::core::ecs::Component(this)
        , x(x)
        , y(y)
    {
    }

    Speed(Speed&&) noexcept = default;
    Speed(const Speed&) = delete;
    Speed& operator=(Speed&&) = delete;
    Speed& operator=(const Speed&) = delete;
};

BOOST_AUTO_TEST_CASE(gearoenix_core_ecs_world)
{
    using namespace gearoenix::core::ecs;
    World w;

    auto e1 = w.create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    auto e2 = w.create_entity(Position { 6.0, 7.0 });
    auto e3 = w.create_entity(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    auto e4 = w.create_entity(Speed { 12.0, 13.0 });
    auto e5 = w.create_entity();

    const auto components_access = [&]() noexcept {
        {
            auto& p = w.get_component<Position>(e1);
            auto& s = w.get_component<Speed>(e1);
            BOOST_TEST(p.x == 2.0);
            BOOST_TEST(p.y == 3.0);
            BOOST_TEST(s.x == 4.0);
            BOOST_TEST(s.y == 5.0);
        }
        {
            auto& p = w.get_component<Position>(e2);
            BOOST_TEST(p.x == 6.0);
            BOOST_TEST(p.y == 7.0);
        }
        {
            auto& p = w.get_component<Position>(e3);
            auto& s = w.get_component<Speed>(e3);
            BOOST_TEST(p.x == 8.0);
            BOOST_TEST(p.y == 9.0);
            BOOST_TEST(s.x == 10.0);
            BOOST_TEST(s.y == 11.0);
        }
        {
            auto& s = w.get_component<Speed>(e4);
            BOOST_TEST(s.x == 12.0);
            BOOST_TEST(s.y == 13.0);
        }
    };

    components_access();

    const auto execute_all_systems = [&]() noexcept {
        w.parallel_system((std::function<void(Entity::id_t, Position&)>)[&](Entity::id_t ent, Position & p) {
            if (e1 == ent) {
                BOOST_TEST(2.0 == p.x);
                BOOST_TEST(3.0 == p.y);
            } else if (e2 == ent) {
                BOOST_TEST(6.0 == p.x);
                BOOST_TEST(7.0 == p.y);
            } else if (e3 == ent) {
                BOOST_TEST(8.0 == p.x);
                BOOST_TEST(9.0 == p.y);
            } else {
                BOOST_TEST(false);
            }
        });
        w.parallel_system((std::function<void(Entity::id_t, Not<Speed>&)>)[&](Entity::id_t ent, Not<Speed>&) {
            BOOST_TEST((e2 == ent || e5 == ent));
        });
        w.parallel_system((std::function<void(Entity::id_t, Not<Position>&)>)[&](Entity::id_t ent, Not<Position>&) {
            BOOST_TEST((e4 == ent || e5 == ent));
        });
        w.parallel_system((std::function<void(Entity::id_t, Speed&)>)[&](Entity::id_t ent, Speed & s) {
            if (e1 == ent) {
                BOOST_TEST(4.0 == s.x);
                BOOST_TEST(5.0 == s.y);
            } else if (e3 == ent) {
                BOOST_TEST(10.0 == s.x);
                BOOST_TEST(11.0 == s.y);
            } else if (e4 == ent) {
                BOOST_TEST(12.0 == s.x);
                BOOST_TEST(13.0 == s.y);
            } else {
                BOOST_TEST(false);
            }
        });
        w.parallel_system(
            (std::function<void(Entity::id_t, Speed&, Position&)>)[&](Entity::id_t ent, Speed & s, Position & p) {
                if (e1 == ent) {
                    BOOST_TEST(2.0 == p.x);
                    BOOST_TEST(3.0 == p.y);
                    BOOST_TEST(4.0 == s.x);
                    BOOST_TEST(5.0 == s.y);
                } else if (e3 == ent) {
                    BOOST_TEST(8.0 == p.x);
                    BOOST_TEST(9.0 == p.y);
                    BOOST_TEST(10.0 == s.x);
                    BOOST_TEST(11.0 == s.y);
                } else {
                    BOOST_TEST(false);
                }
            });
        w.parallel_system(
            (std::function<void(Entity::id_t, Not<Speed>&, Position&)>)[&](Entity::id_t ent, Not<Speed>&,
                Position & p) {
                if (e2 == ent) {
                    BOOST_TEST(6.0 == p.x);
                    BOOST_TEST(7.0 == p.y);
                } else {
                    BOOST_TEST(false);
                }
            });
        w.parallel_system((std::function<void(Entity::id_t, Speed&, Not<Position>&)>)[&](Entity::id_t ent, Speed & s,
            Not<Position>&) {
            if (e4 == ent) {
                BOOST_TEST(12.0 == s.x);
                BOOST_TEST(13.0 == s.y);
            } else {
                BOOST_TEST(false);
            }
        });
    };

    execute_all_systems();

    w.remove_entity(e1);
    w.remove_entity(e2);
    w.remove_entity(e3);
    w.remove_entity(e4);
    w.remove_entity(e5);
}

#endif